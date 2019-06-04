#ifndef HIT_G_H
#define HIT_G_H
  class Hit
  {
    public:
    __device__ Hit():m_eta_x(0.),m_phi_y(0.),m_z(0.),m_E(0.),m_useXYZ(false),m_center_r(0.),m_center_z(0.),m_center_eta(0.),m_center_phi(0.) {}; // for hits with the same energy, m_E should normalized to E(layer)/nhit
    __device__ Hit(float eta, float phi, float E):m_eta_x(eta),m_phi_y(phi),m_E(E),m_useXYZ(false),m_center_r(0.),m_center_z(0.),m_center_eta(0.),m_center_phi(0.) {};
    __device__ Hit(float x, float y, float z, float E):m_eta_x(x),m_phi_y(y),m_z(z),m_E(E),m_useXYZ(true),m_center_r(0.),m_center_z(0.),m_center_eta(0.),m_center_phi(0.) {};

    __device__ inline void setEtaPhiZE(float eta,float phi,float z, float E){
      m_eta_x=eta;
      m_phi_y=phi;
      m_z=z;
      m_E=E;
      m_useXYZ=false;
    }
    __device__ inline void setXYZE(float x,float y,float z, float E){
      m_eta_x=x;
      m_phi_y=y;
      m_z=z;
      m_E=E;
      m_useXYZ=true;
    }

    __device__ inline void reset(){
      m_eta_x=0.;
      m_phi_y=0.;
      m_z=0.;
      m_E=0.;
      m_useXYZ=false;
    }

    __device__ inline float& eta() {return m_eta_x;};
    __device__ inline float& phi() {return m_phi_y;};
    __device__ inline float& x() {return m_eta_x;};
    __device__ inline float& y() {return m_phi_y;};
    __device__ inline float& E() {return m_E;};
    __device__ inline float& z() {return m_z;}
    __device__ inline float r() {
      if(m_useXYZ) return sqrt(m_eta_x*m_eta_x + m_phi_y*m_phi_y);
      else return m_z/sinh(m_eta_x);
    }
    __device__ inline float& center_r(){return m_center_r;}
    __device__ inline float& center_z(){return m_center_z;}
    __device__ inline float& center_eta(){return m_center_eta;}
    __device__ inline float& center_phi(){return m_center_phi;}
    __device__ inline void setCenter_r(float r){m_center_r=r;}
    __device__ inline void setCenter_z(float z){m_center_z=z;}
    __device__ inline void setCenter_eta(float eta){m_center_eta=eta;}
    __device__ inline void setCenter_phi(float phi){m_center_phi=phi;}

    private:
    float m_eta_x; // eta for barrel and end-cap, x for FCal
    float m_phi_y; // phi for barrel and end-cap, y for FCal
    float m_z;
    float m_E;
    bool m_useXYZ;
    // Variables used to store extrapolated position
    float m_center_r;
    float m_center_z;
    float m_center_eta;
    float m_center_phi;
};

#endif