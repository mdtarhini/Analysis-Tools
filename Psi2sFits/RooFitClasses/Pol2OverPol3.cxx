

#include "Riostream.h"

#include "Pol2OverPol3.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include <math.h>
#include "TMath.h"

 Pol2OverPol3::Pol2OverPol3(const char *name, const char *title,
                        RooAbsReal& _x,
                        RooAbsReal& _a0,
                        RooAbsReal& _a1,
                        RooAbsReal& _a2,
                        RooAbsReal& _b1,
                        RooAbsReal& _b2,
                        RooAbsReal& _b3) :
   RooAbsPdf(name,title),
   x("x","x",this,_x),
   a0("a0","a0",this,_a0),
   a1("a1","a1",this,_a1),
   a2("a2","a2",this,_a2),
   b1("b1","b1",this,_b1),
   b2("b2","b2",this,_b2),
   b3("b3","b3",this,_b3)

 {
 }


 Pol2OverPol3::Pol2OverPol3(const Pol2OverPol3& other, const char* name) :
   RooAbsPdf(other,name),
   x("x",this,other.x),
   a0("a0",this,other.a0),
   a1("a1",this,other.a1),
   a2("a2",this,other.a2),
   b1("b1",this,other.b1),
   b2("b2",this,other.b2),
   b3("b3",this,other.b3)
 {
 }


 Double_t Pol2OverPol3::evaluate() const
 {
   Double_t r =(a0 + a1*x + a2*x*x )/(1+ b1*x +b2*x*x + b3*x*x*x);
   if(r>0)
   return r;
   else
   return 0;

 }
