/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OMP_BIGMEM_H
#define OMP_BIGMEM_H

#include <omp.h>
#include <vector>

class OMP_BigMem  {

public :
	OMP_BigMem(): m_seg_size(0), m_ptrs(0), m_used(0), m_seg(0)   { } ; 
	OMP_BigMem( size_t s ) {  //initialize to one seg with size s
	  void* p ;  
	  m_seg_size = s ;
	  //gpuQ(cudaMalloc(&p , m_seg_size )) ;
          //CHECK void ATIF   
	  p = (void *) omp_target_alloc( m_seg_size, m_default_device);
	  if ( p == NULL ) {
            std::cout << " ERROR: No space left on device for p in big mem." << std::endl;
          }
          m_ptrs.push_back(p) ; 
	//  bm_ptr = self ;
	  m_seg =0 ;
	  m_used.push_back(0)  ;
//std::cout<<"zzz: " <<m_seg_size<<",p= " << p<<"," <<m_ptrs[0] << std::endl ;
	 } ; 
	~OMP_BigMem() {
		for(int i=0 ; i<m_ptrs.size() ; i++) omp_target_free ( m_ptrs[i], m_default_device ); 
	}  ;


	void * omp_bm_alloc(size_t s) {
	  if (s  > (m_seg_size-m_used[m_seg]))  add_seg() ;
		long * q = (long *) m_ptrs[m_seg] ;
		int offset = m_used[m_seg]/sizeof(long) ;
	  	void * p = (void * )   &(q[offset])  ;
		m_used[m_seg] += ((s+sizeof(long)-1)/sizeof(long)  ) * sizeof(long)    ;
		return p  ;
	};

	size_t  size() { return (m_seg+1) * m_seg_size ; } ;
	size_t  used() { return m_seg * m_seg_size + m_used[m_seg] ; };

	static OMP_BigMem * bm_ptr  ;
	///static void set_bm_ptr( OMP_BigMem* p) {bm_ptr= p ; } ;

	//static  OMP_Big_Mem* get_bm_ptr( ) { return bm_ptr ; } ;

private : 
	int m_default_device = omp_get_default_device();
        int m_initial_device = omp_get_initial_device();
        std::size_t m_offset = 0;

        void add_seg() { 
		void * p ; 
		//gpuQ(cudaMalloc((void**)&p , m_seg_size )) ;
   	        p = (void *) omp_target_alloc( m_seg_size, m_default_device);
                if ( p == NULL ) {
                  std::cout << " ERROR: No space left on device for p in big mem." << std::endl;
                }
		m_ptrs.push_back(p) ;
		m_seg++;
		m_used.push_back(0)  ;
	};
		
	std::vector<void*>  m_ptrs  ;  //points to each allocated segment 
	int  m_seg ;  //do we need ? it's current size of mptrs -1 ;
	size_t   m_seg_size  ; // size of each allocation
	std::vector < size_t>  m_used ; // used memory in each allocated segment
} ;




#endif