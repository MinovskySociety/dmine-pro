#ifndef MATCH_SEMANTICS
#define MATCH_SEMANTICS

namespace GUNDAM {

enum MatchSemantics : bool { 
  kIsomorphism, 
  kHomomorphism 
};

enum MatchAlgorithm : bool {
  kVf2,
  kDagDp
};

};

#endif // MATCH_SEMANTICS