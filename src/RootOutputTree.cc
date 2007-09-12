#include "RootOutputTree.h"
#include "TFile.h"
#include "FWCore/Utilities/interface/WrappedClassName.h"

#include <algorithm>

namespace edm {

  TTree *
  RootOutputTree::makeTree(TFile * filePtr,
			   std::string const& name,
			   int splitLevel,
			   TChain * chain,
			   Selections const&) {
    TTree *tree;
    if (chain != 0) {
      tree = new TTree(name.c_str(), "", splitLevel);
    } else {
      tree = new TTree(name.c_str(), "", splitLevel);
    }
    tree->SetDirectory(filePtr);
    return tree;
    
  }

  void
  RootOutputTree::writeTTree(TTree *tree) {
    tree->AutoSave();
  }

  void
  RootOutputTree::fillTTree(TTree * tree, std::vector<TBranch *> const& branches) {
    for_each(branches.begin(), branches.end(), fillHelper);
    if (tree->GetNbranches() != 0) {
      tree->SetEntries(-1);
    }
  }

  void
  RootOutputTree::writeTree() const {
    writeTTree(tree_);
    writeTTree(metaTree_);
  }

  void RootOutputTree::fillTree() const {
    fillTTree(metaTree_, metaBranches_);
    fillTTree(tree_, branches_);
  }

  void
  RootOutputTree::addBranch(BranchDescription const& prod, bool selected, BranchEntryDescription const*& pProv, void const*& pProd) {
      prod.init();
      TBranch * meta = metaTree_->Branch(prod.branchName().c_str(), &pProv, basketSize_, 0);
      metaBranches_.push_back(meta);
      if (selected) {
	TBranch * branch = tree_->Branch(prod.branchName().c_str(),
		       wrappedClassName(prod.className()).c_str(),
		       &pProd,
		       (prod.basketSize() == BranchDescription::invalidBasketSize ? basketSize_ : prod.basketSize()),
		       (prod.splitLevel() == BranchDescription::invalidSplitLevel ? splitLevel_ : prod.splitLevel()));
        branches_.push_back(branch);
	// we want the new branch name for the JobReport
	branchNames_.push_back(prod.branchName());
      }
  }
}
