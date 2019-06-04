#include "CaloGpuGeneral.h"
#include "GeoRegion.cu"
#include "Hit.h"

#include "gpuQ.h"
#include "Args.h"

#define BLOCK_SIZE 1024 
#define NLOOPS 2

__device__  long long getDDE( GeoGpu* geo, int sampling, float eta, float phi) {

   float * distance = 0 ;
   int * steps =0 ;

int MAX_SAMPLING = geo->max_sample ;
Rg_Sample_Index * SampleIdx = geo->sample_index ;
 GeoRegion * regions_g = geo->regions ;

if(sampling<0) return -1;
  if(sampling>=MAX_SAMPLING) return -1;

   int sample_size= SampleIdx[sampling].size ;
   int sample_index=SampleIdx[sampling].index ;

   GeoRegion * gr = ( GeoRegion *) regions_g ; 
   printf("gr * is %d\n", gr) ; 
  if(sample_size==0) return -1;
  float dist;
  long long bestDDE=-1;
  if(!distance) distance=&dist;
  *distance=+10000000;
  int intsteps;
  int beststeps;
  if(steps) beststeps=(*steps);
   else beststeps=0;

  
  if(sampling<21) {
    for(int skip_range_check=0;skip_range_check<=1;++skip_range_check) {
      for(unsigned int j= sample_index; j< sample_index+sample_size ; ++j) {
        if(!skip_range_check) {
          if(eta< gr[j].mineta()) continue;
          if(eta> gr[j].maxeta()) continue;
        }
    if(steps) intsteps=(*steps);
         else 
       intsteps=0;
        float newdist;
        long long  newDDE= gr[j].getDDE(eta,phi,&newdist,&intsteps);
        if(newdist<*distance) {
          bestDDE=newDDE;
          *distance=newdist;
          if(steps) beststeps=intsteps;
          if(newdist<-0.1) break; //stop, we are well within the hit cell
       }
      }
      if(bestDDE>=0) break;
  }
  } else {
                return -3;
  }
  if(steps) *steps=beststeps;

  return bestDDE;
}

__global__  void testHello_xxx() {

printf("Hello..., I am from GPU threadi..... %d\n", threadIdx.x);

};


__global__  void test_getDDE(GeoGpu * geo , int sample, float eta, float phi) {

long long  index=getDDE(geo, sample, eta,phi) ;

printf("From GPU index of the cell with eta=%f, phi=%f is %ld \n", eta, phi, index) ;

}


__global__  void test_rand(float * r ) {
  int  tid=blockIdx.x*blockDim.x+threadIdx.x ;

  printf("Tid%d  Block ID %d, Thread %d r=[%f]\n", 
         tid, blockIdx.x, threadIdx.x, r[tid] ) ; 



}

__host__ void CaloGpuGeneral::Gpu_Chain_Test() {

        std::cout<< " calling testHelloixxx()"<< std::endl;
 testHello_xxx <<<1, 1>>> () ;
    cudaDeviceSynchronize() ;

 cudaError_t err = cudaGetLastError();
 if (err != cudaSuccess) {
        std::cout<<" testHello "<< cudaGetErrorString(err)<< std::endl;

}

 test_getDDE <<<1,1>>> (GeoLoadGpu::Geo_g,  5, -1.5, 1.6 ) ;
    cudaDeviceSynchronize() ;

 err = cudaGetLastError();
 if (err != cudaSuccess) {
        std::cout<< "test_getDEE "<<cudaGetErrorString(err)<< std::endl;

}


// random number test
     std::cout<<"test Random"<<std::endl ;
     Rand4Hits *  rd4h = new Rand4Hits ; 
     float * r= rd4h->HitsRandGen(34, 1234ULL) ;
     test_rand <<< 10, 10 >>> (r ) ;
  cudaDeviceSynchronize() ;
 err = cudaGetLastError();
if (err != cudaSuccess) {
        std::cout<<" testRandom "<< cudaGetErrorString(err)<< std::endl;
     delete rd4h  ;

}

}


__device__ void  rnd_to_fct2d(float& valuex,float& valuey,float rnd0,float rnd1, FH2D* hf2d) {

 int nbinsx=(*hf2d).nbinsx;
 int nbinxy=(*hf2d).nbinsy;
 float * HistoContents= (*hf2d).h_contents ;
 float* HistoBorders= (*hf2d).h_bordersx ;
 float* HistoBordersy= (*hf2d).h_bordersy ; 

 int ibin = nbinsx*nbinxy-1 ;
 for ( int i=0 ; i < nbinsx*nbinxy ; ++i) {
    if   (HistoContents[i]> rnd0 ) {
	 ibin = i ;
	 break ;
	}
 } 

  int biny = ibin/nbinsx;
  int binx = ibin - nbinsx*biny;

  float basecont=0;
  if(ibin>0) basecont=HistoContents[ibin-1];

  float dcont=HistoContents[ibin]-basecont;
  if(dcont>0) {
    valuex = HistoBorders[binx] + (HistoBorders[binx+1]-HistoBorders[binx]) * (rnd0-basecont) / dcont;
  } else {
    valuex = HistoBorders[binx] + (HistoBorders[binx+1]-HistoBorders[binx]) / 2;
  }
  valuey = HistoBordersy[biny] + (HistoBordersy[biny+1]-HistoBordersy[biny]) * rnd1;

}


__device__  float  rnd_to_fct1d( float  rnd, uint32_t* contents, float* borders , int nbins, uint32_t s_MaxValue  ) {


  uint32_t int_rnd=s_MaxValue*rnd;

  int  ibin=nbins-1 ;
  for ( int i=0 ; i < nbins ; ++i) {
    if   (contents[i]> int_rnd ) {
         ibin = i ;
         break ;
        }
  }

  int binx = ibin;

  uint32_t basecont=0;
  if(ibin>0) basecont=contents[ibin-1];

  uint32_t dcont=contents[ibin]-basecont;
  if(dcont>0) {
    return borders[binx] + ((borders[binx+1]-borders[binx]) * (int_rnd-basecont)) / dcont;
  } else {
    return borders[binx] + (borders[binx+1]-borders[binx]) / 2;
  }

}



__device__  void CenterPositionCalculation_d(Hit* hit, const Chain0_Args args) {

    hit->setCenter_r((1.- args.extrapWeight)*args.extrapol_r_ent + 
	args.extrapWeight*args.extrapol_r_ext) ;
    hit->setCenter_z((1.- args.extrapWeight)*args.extrapol_z_ent + 
	args.extrapWeight*args.extrapol_z_ext) ;
    hit->setCenter_r((1.- args.extrapWeight)*args.extrapol_eta_ent + 
	args.extrapWeight*args.extrapol_eta_ext) ;
    hit->setCenter_z((1.- args.extrapWeight)*args.extrapol_phi_ent + 
	args.extrapWeight*args.extrapol_phi_ext) ;
}


__device__ void ValidationHitSpy_d( Hit* hit, const  Chain0_Args args ) {


}

__device__ void HistoLateralShapeParametrization_d( Hit* hit, unsigned long t, Chain0_Args args ) {

  int     pdgId    = args.pdgId;
  double  charge   = args.charge;

  int cs=args.charge;
  double center_eta = hit->center_eta();
  double center_phi = hit->center_phi();
  double center_r   = hit->center_r();
  double center_z   = hit->center_z();


  float alpha, r, rnd1, rnd2;
  rnd1 = args.rand[t];
  rnd2 = args.rand[t+args.nhits];
  if(args.is_phi_symmetric) {
    if(rnd2>=0.5) { //Fill negative phi half of shape
      rnd2-=0.5;
      rnd2*=2;
      rnd_to_fct2d(alpha,r,rnd1,rnd2,args.fh2d);
      alpha=-alpha;
    } else { //Fill positive phi half of shape
      rnd2*=2;
      rnd_to_fct2d(alpha,r,rnd1,rnd2,args.fh2d);
    }
  } else {
    rnd_to_fct2d(alpha,r,rnd1,rnd2,args.fh2d);
  }

  float delta_eta_mm = r * cos(alpha);
  float delta_phi_mm = r * sin(alpha);

  // Particles with negative eta are expected to have the same shape as those with positive eta after transformation: delta_eta --> -delta_eta
  if(center_eta<0.)delta_eta_mm = -delta_eta_mm;
  // Particle with negative charge are expected to have the same shape as positively charged particles after transformation: delta_phi --> -delta_phi
  if(charge < 0.)  delta_phi_mm = -delta_phi_mm;

  float dist000    = sqrt(center_r * center_r + center_z * center_z);
  float eta_jakobi = abs(2.0 * exp(-center_eta) / (1.0 + exp(-2 * center_eta)));

  float delta_eta = delta_eta_mm / eta_jakobi / dist000;
  float delta_phi = delta_phi_mm / center_r;

  hit->setEtaPhiZE(center_eta + delta_eta,center_phi + delta_phi,center_z, hit->E());


}

__device__ void HitCellMapping_d( Hit* hit,unsigned long t, Chain0_Args args ) {


 
// long long  cellele= getDDE(args.cs,hit->eta(),hit->phi());
if (( t == (args.nhits-1))||t==0 ) 
	printf("In HitCellMapping_d, cs=%d, nhits=%d hit:  %f,%f, %f,%f \n",
		 args.cs,args.nhits, hit->eta(),hit->phi(), hit->z(), hit->E() ); 


}

__device__ void HitCellMappingWiggle_d( Hit* hit,  Chain0_Args args, unsigned long  t ) {

 int nhist=(*(args.fhs)).nhist;
 float*  bin_low_edge = (*(args.fhs)).low_edge ;
 

 float eta =fabs( hit->eta()); 
  if (( t == (args.nhits-1))||t==0 )  printf("In HitCellMappingWiggle_d t=%d, bin_low_edge[0]=%f, bin_low_edge[1]=%f, eta=%f\n",t, bin_low_edge[0], bin_low_edge[1], eta);
 if(eta<bin_low_edge[0] || eta> bin_low_edge[nhist]) {
   HitCellMapping_d(hit, t, args) ;

 }

 int bin= nhist ;
  for (int i =0; i< nhist+1 ; ++i ) {
 	if(bin_low_edge[i] > eta ) {
	  bin = i ;
	  break ;
	}
  }

  bin -= 1; 

  uint32_t * contents = (*(args.fhs)).h_contents[bin] ;
  float* borders = (*(args.fhs)).h_borders[bin] ;
  

     float rnd= args.rand[t+2*args.nhits];

    double wiggle=rnd_to_fct1d(rnd,contents, borders, nhist, (*(args.fhs)).s_MaxValue);


    double hit_phi_shifted=hit->phi()+wiggle;
    hit->phi()=Phi_mpi_pi(hit_phi_shifted);
  if (( t == (args.nhits-1))||t==0 )  printf("In HitCellMappingWiggle_d t=%ld, bin=%d, rand=%f,wiggle=%f, hit.phi=%f,shit=%f,contents[0]=%f,borders[0]=%f \n",
		 t, bin, rnd,wiggle, hit->phi(),  hit_phi_shifted, contents[0],borders[0]);
  

  HitCellMapping_d(hit, t,  args) ;

}


__global__  void simulate_chain0_A( float E, int nhits,  Chain0_Args args ) {

  int tid = threadIdx.x + blockIdx.x*blockDim.x ;
  for ( int i=0 ; i<NLOOPS ; ++i ) { 
    unsigned long t = tid+i*gridDim.x*blockDim.x ;
    if ( t  >= nhits ) break ;  
    Hit* hit  =new Hit() ;
    hit->E()=E ;
    CenterPositionCalculation_d( hit, args) ;
    ValidationHitSpy_d( hit, args) ;
    HistoLateralShapeParametrization_d(hit,t,  args) ;
   if ( t==(nhits-1)|| t==0 ) printf("t=%ld, nhits=%d, %d, hit.eta=%f hitE=%f\n", t, nhits, args.nhits, hit->eta(),hit->E()) ;
    HitCellMappingWiggle_d ( hit, args, t ) ;
    ValidationHitSpy_d(hit,args);
// do something 
    delete hit ;
  }
 
}




__global__  void simulate_chain0_B() {

	printf("From kernel simulate_chain0_B:\n" );

}
__global__  void simulate_chain0_C() {

	printf("From kernel simulate_chain0_C:\n" );
}


__host__ void CaloGpuGeneral::simulate_hits(float E, int nhits, Chain0_Args args ) {

	Rand4Hits *  rd4h = new Rand4Hits ;
        float * r= rd4h->HitsRandGen(nhits, args.seed) ;
         args.rand = r ;
	 std::cout << "rand pointer " << r << std::endl ;


	int blocksize=BLOCK_SIZE ;
	int threads_tot= (nhits +NLOOPS-1) /NLOOPS  ;
  	
	int nblocks= (threads_tot + blocksize-1 )/blocksize ;        

	 std::cout<<"Nblocks: "<< nblocks << ", blocksize: "<< blocksize 
                << ", total Threads: " << threads_tot << std::endl ;
  simulate_chain0_A <<<nblocks, blocksize  >>> (E, nhits, args  ) ; 
  cudaDeviceSynchronize() ;
 cudaError_t err = cudaGetLastError();
 if (err != cudaSuccess) {
        std::cout<< "simulate_chain0_A "<<cudaGetErrorString(err)<< std::endl;

}


  simulate_chain0_B <<<1,1 >>> () ;
  cudaDeviceSynchronize() ;
 err = cudaGetLastError();
 if (err != cudaSuccess) {
        std::cout<< "simulate_chain0_B "<<cudaGetErrorString(err)<< std::endl;

}
  simulate_chain0_C <<< 1,1 >>> () ;
  cudaDeviceSynchronize() ;
 err = cudaGetLastError();
 if (err != cudaSuccess) {
        std::cout<< "simulate_chain0_C "<<cudaGetErrorString(err)<< std::endl;

}

	delete rd4h ;

}



