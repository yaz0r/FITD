#ifndef _TRIANGLE_POLYTRY_
#define _TRIANGLE_POLYTRY_

class Vector3F
{
public:
      Vector3F(float x=0.0f,float y=0.0f,float z=0.0f)   {  Set(x,y,z); }
      Vector3F(const Vector3F& point)                    {  Set(point.m_v[0],point.m_v[1],point.m_v[2]); }
      //
      void  Set(float x,float y,float z)                 {  m_v[0]=x; m_v[1]=y;   m_v[2]=z; }
      //
      float       operator []( int i) const              {  return m_v[i]; }
      float&      operator []( int i)                    {  return m_v[i]; }
      //
      Vector3F&   operator +=(const Vector3F& v)         {  m_v[0]+=v.m_v[0];  m_v[1]+=v.m_v[1];  m_v[2]+=v.m_v[2];  return *this;  }
      Vector3F&   operator -=(const Vector3F& v)         {  m_v[0]-=v.m_v[0];  m_v[1]-=v.m_v[1];  m_v[2]-=v.m_v[2];  return *this;  }
      Vector3F&   operator *=(const float d)             {  m_v[0]*=d;  m_v[1]*=d;  m_v[2]*=d;  return *this;  }
      //
      float       Dot(const Vector3F& v)        const    {  return (m_v[0]*v.m_v[0]+m_v[1]*v.m_v[1]+m_v[2]*v.m_v[2]);   }
      void        Vector(const Vector3F& b,
                               Vector3F &c)     const    {  c.m_v[0]= m_v[1] * b.m_v[2] - m_v[2] * b.m_v[1];
	                                                         c.m_v[1]= m_v[2] * b.m_v[0] - m_v[0] * b.m_v[2];
	                                                         c.m_v[2]= m_v[0] * b.m_v[1] - m_v[1] * b.m_v[0]; }
      float       SquaredLenght()               const    {  return Dot(*this);   }
      float       Lenght()                      const    {  return (float)sqrt(Dot(*this));   }
      //
      float m_v[3];
};

class CPolyTri
{
public:
      CPolyTri()              {  m_nIndex=NULL; m_nMaxPoints=0;      }
      virtual  ~CPolyTri()    {  if( m_nIndex ) delete [] m_nIndex;  }
      //
      // Sel closed...
      //
      int   Triangulate(const Vector3F* points,const Vector3F &normal,int nCount)
               {
                  //
                  // Alloca un vettore di interi ...
                  //
                  int   nTriangle= 0;
                  int   nVertex  = nCount;
                  //
                  AllocIndex(nCount);
                  //
                  bool     bNoErrors   = true;
                  //
                  while( nVertex > 3 && bNoErrors ){
                     //
                     // tri to remove one vertex...
                     //
                     bNoErrors   = false;
                     //
                     for( int i=0 , j=1 , k=2 ; k < nVertex ; ){
                        //
                        switch( TriangleArea(points      ,
                                             m_nIndex[i] ,
                                             m_nIndex[j] ,
                                             m_nIndex[k] ,
                                             normal      ) ){
                           //
                           // ok. flush face && remove vertex j
                           //
                           case convex       :
                              //
                              // Testing containement
                              //
                              if( IsAnyPointInside(points,i,j,k,nVertex) ){
                                 //
                                 // go ahead..
                                 //
                                 i  = j;
                                 j  = k;
                                 k++;
                              }else{
                                 nTriangle++;
                                 AddFace(points,m_nIndex[i],m_nIndex[j],m_nIndex[k]);
                                 //
                                 // remove vertex j
                                 //
                                 nVertex  = RemoveVertex( j,nVertex );
                              bNoErrors= true;
                              }
                              break;
                           case concave      :
                              //
                              // go ahead..
                              //
                              i  = j;
                              j  = k;
                              k++;
                              break;
                           case degenerate   :
                              //
                              // remove vertex j
                              //
                              nVertex  = RemoveVertex( j,nVertex );
                              bNoErrors= true;
                              break;
                        }
                     }
                  }
                  return nTriangle;
               }
      //
      // Utility Polygon Normal...
      //
      static   void  ComputeNormal(const Vector3F* points,int nPoints,Vector3F &normal)
               {
                  //
                  // Newell    
                  //
                  normal[0]=normal[1]=normal[2]=0.0f;
                  for( register int i=0 , j=1 ; j < nPoints ; i++ , j++ ){
                     normal[0]+= ( points[i][1] - points[j][1] ) * ( points[i][2] + points[j][2] ) ;
                     normal[1]+= ( points[i][2] - points[j][2] ) * ( points[i][0] + points[j][0] ) ;
                     normal[2]+= ( points[i][0] - points[j][0] ) * ( points[i][1] + points[j][1] ) ;
                  }
               }
      //
      // Utility Test Polygon Convexity ...
      //
      static   bool  IsConvex(const Vector3F* points,int nPoints,const Vector3F &normal)
               {
                  //
                  // calcolo del versore se il poligono e' convesso
                  // il prodotto vettoriale 
                  //
                  Vector3F vi( points[1] );   vi-=points[0];
                  Vector3F vj,n;
                  int      nP=nPoints-1;
                  //
                  for( register int i=0 , j=1 , k ; j < nPoints ; i++ , j++ ){
                     k  = (j+1) % nP;
                     vj = points[k];
                     vj-= points[j];
                     //
                     vi.Vector(vj,n);
                     //
                     if( (n[0]*normal[0]) < 0.0f ||
                         (n[1]*normal[1]) < 0.0f ||
                         (n[2]*normal[2]) < 0.0f ) break;
                     vi = vj;
                  }
                  return ( j == nPoints ? true : false );
               }
protected:
      //
      int         *m_nIndex;
      Vector3F    m_e0     ;
      Vector3F    m_e1     ;
      Vector3F    m_N      ;
      float       m_A      ;  // 2 area
      //
      enum  {  convex      =  1,
               degenerate  =  0,
               concave     = -1};
      //
      int   RemoveVertex( int j,int nVertex )
            {
               while( ++j < nVertex )
                  m_nIndex[j-1]=m_nIndex[j];
               return (nVertex-1);
            }
      //
      bool  IsAnyPointInside(const Vector3F* points,int i,int j,int k,int nVertex) const
               {
                  int   ik=m_nIndex[k];
                  for( register int ip=0 ; ip < nVertex ; ip++ )
                     if( ( ip < i || ip > k ) &&
                        IsPointInside(points[m_nIndex[ip]],points[ik]) ){
                        return true;
                     }
                  return false;
               }
      //
      bool  IsPointInside(const Vector3F  point,const Vector3F  q2)  const
            {
               Vector3F pmq2  = point;
                        pmq2 -= q2;
               //
               Vector3F ntmp;
               //
               float    B0,B1;
               //
               pmq2.Vector(m_e1,ntmp);  if( (B0=m_N.Dot(ntmp)) <= 0.0 ) return false;
               m_e0.Vector(pmq2,ntmp);  if( (B1=m_N.Dot(ntmp)) <= 0.0 ) return false;
               return ( (m_A-B0-B1) > 0.0 ? true : false );
            }
      //
      int   TriangleArea(const Vector3F* points,int i,int j,int k,const Vector3F& normal)
            {
               m_e0=points[i];  m_e0-=points[k];
               m_e1=points[j];  m_e1-=points[k];
               //
               m_e0.Vector(m_e1,m_N);
               //
               m_A=m_N.SquaredLenght();
               //
               // j is alligned from i to k ?
               //
               if( (-FLT_EPSILON) < m_A && m_A < FLT_EPSILON )
                  return degenerate;
               //
               // test convexity :
               //
               return ( m_N.Dot( normal ) < 0.0 ? concave : convex );
            }
      //
      virtual  void  AddFace(const Vector3F* points,int i,int j,int k)
            {}
private:
	   int	m_nMaxPoints;
      void  AllocIndex(int nCount)
            {
               if( nCount > m_nMaxPoints ){
                  if( m_nIndex ) delete [] m_nIndex;
                  m_nMaxPoints   = nCount+2;
                  m_nIndex       = new int[m_nMaxPoints];
               }
               for( register int i=0 ; i < nCount ; i++ )
                  m_nIndex[i] = i;
            }
};
//
class CTriDC : public CPolyTri
{
public:
      CTriDC(CDC* pDC)
         {
            m_pDC = pDC;
         }
      CDC*  m_pDC;
protected:
      //
      CPoint   m_points[4];
      //
      virtual  void  AddFace(const Vector3F* points,int i,int j,int k)
            {
               m_points[0].x  =(int)points[i][0];  m_points[0].y  =(int)points[i][1];
               m_points[1].x  =(int)points[j][0];  m_points[1].y  =(int)points[j][1];
               m_points[2].x  =(int)points[k][0];  m_points[2].y  =(int)points[k][1];
               m_points[3]    = m_points[0];
               m_pDC->Polygon(m_points,4);
            }
};


#define _TRIANGLE_POLYTRY_
#endif
