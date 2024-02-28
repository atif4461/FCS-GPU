#include "CaloGpuGeneral_al.h"
#include "GeoRegion.h"
#include "GeoGpu_structs.h"
#include "Hit.h"
#include "Rand4Hits.h"
#include "AlpakaDefs.h"

#include "Args.h"
#include <chrono>
#include <climits>
#include <mutex>

#define DEFAULT_BLOCK_SIZE 256

#define M_PI 3.14159265358979323846
#define M_2PI 6.28318530717958647692

using namespace CaloGpuGeneral_fnc;

static std::once_flag calledGetEnv{};
static int BLOCK_SIZE{ DEFAULT_BLOCK_SIZE };

static int count{ 0 };

static CaloGpuGeneral::KernelTime timing;

namespace CaloGpuGeneral_al {

  __HOST__ void Rand4Hits_finish(void *rd4h) {
    
    if((Rand4Hits *)rd4h) delete (Rand4Hits *)rd4h;
    
    if (timing.count > 0) {
      std::cout << "kernel timing\n";
      std::cout << timing;
    } else {
      std::cout << "no kernel timing available" << std::endl;
    }
  }


  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  struct SimulateHitsDeKernel
  {
    template<typename TAcc, typename Sim_Args>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc
                                  , Sim_Args args
                                  ) const -> void
    {
      auto const idx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
      if(idx < args.nhits) {
        Hit hit;

	int bin = find_index_long(args.simbins, args.nbins, idx);
	HitParams hp = args.hitparams[bin];
	hit.E() = hp.E;

	CenterPositionCalculation_g_d(hp, hit, idx, args);
	HistoLateralShapeParametrization_g_d(hp, hit, idx, args);
	if (hp.cmw)
	  HitCellMappingWiggle_g_d(acc, hp, hit, idx, args);
	HitCellMapping_g_d(acc, hp, hit, idx, args);
      }
    }
  };

  auto simulate_hits_de_alpaka(Sim_Args& args, QueueAcc& queue) -> void {

    int blocksize = BLOCK_SIZE;
    int threads_tot = args.nhits;
    int nblocks = (threads_tot + blocksize - 1) / blocksize;

    WorkDiv workdiv{static_cast<Idx>(nblocks)
        , static_cast<Idx>(blocksize)
        , static_cast<Idx>(1)};


    SimulateHitsDeKernel simulateHitsDe;
    alpaka::exec<Acc>(queue
                      , workdiv
                      , simulateHitsDe
                      , args);
    alpaka::wait(queue);
  }
  

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  struct SimulateHitsCtKernel
  {
    template<typename TAcc, typename Sim_Args>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc
                                  , Sim_Args args
                                  ) const -> void
    {
      auto const idx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];
      int sim = idx / args.ncells;
      unsigned long cellid = idx % args.ncells;

      if (idx < args.ncells * args.nsims) {
	if (args.cells_energy[idx] > 0) {
	  unsigned int ct = alpaka::atomicAdd( acc, &args.ct[sim], 1 );
	  Cell_E ce;
	  ce.cellid = cellid;
	  ce.energy = args.cells_energy[idx];
	  args.hitcells_E[ct + sim * MAXHITCT] = ce;
	}
      }
    }  
  };

  auto simulate_hits_ct_alpaka(Sim_Args& args, QueueAcc& queue) -> void {

    int blocksize = BLOCK_SIZE;
    int nblocks = (args.ncells * args.nsims + blocksize - 1) / blocksize;

    WorkDiv workdiv{static_cast<Idx>(nblocks)
        , static_cast<Idx>(blocksize)
        , static_cast<Idx>(1)};


    SimulateHitsCtKernel simulateHitsCt;
    alpaka::exec<Acc>(queue
                      , workdiv
                      , simulateHitsCt
                      , args);
    alpaka::wait(queue);
  }


  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  struct SimulateCleanKernel
  {
    template<typename TAcc, typename Sim_Args>
    ALPAKA_FN_ACC auto operator()(TAcc const& acc
                                  , Sim_Args args
                                  ) const -> void
    {
      auto const idx = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0];

      if (idx < args.ncells * args.nsims) {
	args.cells_energy[idx] = 0.0;
      }
      if (idx < args.nsims) {
	args.ct[idx] = 0;
      }
    }
  };

  auto simulate_clean_alpaka(Sim_Args& args, QueueAcc& queue) -> void {

    int blocksize = BLOCK_SIZE;
    int threads_tot = args.ncells * args.nsims;
    int nblocks = (threads_tot + blocksize - 1) / blocksize;

    WorkDiv workdiv{static_cast<Idx>(nblocks)
        , static_cast<Idx>(blocksize)
        , static_cast<Idx>(1)};

    SimulateCleanKernel simulateClean;
    alpaka::exec<Acc>(queue
                      , workdiv
                      , simulateClean
                      , args);
    alpaka::wait(queue);
  }


  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  __HOST__ void simulate_hits_gr(Sim_Args &args) {

    std::call_once(calledGetEnv, []() {
	if (const char *env_p = std::getenv("FCS_BLOCK_SIZE")) {
	  std::string bs(env_p);
	  BLOCK_SIZE = std::stoi(bs);
	}
	if (BLOCK_SIZE != DEFAULT_BLOCK_SIZE) {
	  std::cout << "kernel BLOCK_SIZE: " << BLOCK_SIZE << std::endl;
	}
      });

    QueueAcc queue(alpaka::getDevByIdx<Acc>(Idx{0}));

    // clean workspace
    auto t0 = std::chrono::system_clock::now();
    simulate_clean_alpaka(args,queue);
    
    // main simulation
    auto t1 = std::chrono::system_clock::now();
    simulate_hits_de_alpaka(args,queue);
    
    // stream compaction
    auto t2 = std::chrono::system_clock::now();
    simulate_hits_ct_alpaka(args,queue);

    // copy back to host
    auto t3 = std::chrono::system_clock::now();
    Rand4Hits * rd4h = (Rand4Hits *)args.rd4h;

    auto bufDevCt = rd4h->get_cT();
    CellCtTHost bufHostCt = alpaka::allocBuf<CELL_CT_T,Idx>(alpaka::getDevByIdx<Host>(0u),Vec{Idx(args.nsims)});
    CELL_CT_T* bufHostCtRaw = alpaka::getPtrNative(bufHostCt);
    alpaka::memcpy(queue,bufHostCt,bufDevCt,Vec{Idx(args.nsims)});
    alpaka::wait(queue);
    memcpy(args.ct_h,bufHostCtRaw,args.nsims*sizeof(CELL_CT_T));

    auto bufDevE = rd4h->get_cell_E();
    CellEHost bufHostE = alpaka::allocBuf<Cell_E,Idx>(alpaka::getDevByIdx<Host>(0u),Vec{Idx(args.nsims*MAXHITCT)});
    Cell_E* bufHostERaw = alpaka::getPtrNative(bufHostE);
    alpaka::memcpy(queue,bufHostE,bufDevE,Vec{Idx(args.nsims*MAXHITCT)});
    alpaka::wait(queue);
    memcpy(args.hitcells_E_h,bufHostERaw,args.nsims*MAXHITCT*sizeof(Cell_E));

    auto t4 = std::chrono::system_clock::now();
    
#ifdef DUMP_HITCELLS
    std::cout << "nsim: " << args.nsims << "\n";
    for (int isim = 0; isim < args.nsims; ++isim) {
      std::cout << "  nhit: " << args.ct_h[isim] << "\n";
      std::map<unsigned int, float> cm;
      for (int ihit = 0; ihit < args.ct_h[isim]; ++ihit) {
	cm[args.hitcells_E_h[ihit + isim * MAXHITCT].cellid] =
          args.hitcells_E_h[ihit + isim * MAXHITCT].energy;
      }
      
      int i = 0;
      for (auto &em : cm) {
	std::cout << "   " << isim << " " << i++ << "  cell: " << em.first << "  "
		  << em.second << std::endl;
      }
    }
#endif

    timing.add(t1 - t0, t2 - t1, t3 - t2, t4 - t3);
  }

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  __HOST__ void load_hitsim_params(void *rd4h, HitParams *hp, long *simbins,
				   int bins)
  {
    if (!(Rand4Hits *)rd4h) {
      std::cerr << "Error load hit simulation params ! ";
      exit(2);
    }

    QueueAcc queue(alpaka::getDevByIdx<Acc>(Idx{0}));

    auto devHitParams = ((Rand4Hits *)rd4h)->get_hitParams();
    BufHostHitParams hostHitParams = alpaka::allocBuf<HitParams,Idx>(alpaka::getDevByIdx<Host>(0u),Vec{Idx(bins)});
    HitParams* hostHitParamsRaw = alpaka::getPtrNative(hostHitParams);
    memcpy(hostHitParamsRaw,hp,bins * sizeof(HitParams));
    alpaka::memcpy(queue,devHitParams,hostHitParams,Vec{Idx(bins)});
    alpaka::wait(queue);

    auto devSimBins = ((Rand4Hits *)rd4h)->get_simBins();
    BufHostLong hostSimBins = alpaka::allocBuf<long,Idx>(alpaka::getDevByIdx<Host>(0u),Vec{Idx(bins)});
    long* hostSimBinsRaw = alpaka::getPtrNative(hostSimBins);
    memcpy(hostSimBinsRaw,simbins,bins * sizeof(long));
    alpaka::memcpy(queue,devSimBins,hostSimBins,Vec{Idx(bins)});
    alpaka::wait(queue);
  }

} // namespace CaloGpuGeneral_cu