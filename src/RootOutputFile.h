#ifndef IOPool_Output_RootOutputFile_h
#define IOPool_Output_RootOutputFile_h

//////////////////////////////////////////////////////////////////////
//
// $Id: RootOutputFile.h,v 1.29.2.3 2008/04/29 07:58:12 wmtan Exp $
//
// Class RootOutputFile
//
// Oringinal Author: Luca Lista
// Current Author: Bill Tanenbaum
//
//////////////////////////////////////////////////////////////////////

#include <map>
#include <string>
#include <vector>

#include "boost/array.hpp"
#include "boost/shared_ptr.hpp"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/JobReport.h"
#include "DataFormats/Provenance/interface/BranchDescription.h"
#include "DataFormats/Provenance/interface/BranchType.h"
#include "DataFormats/Provenance/interface/FileID.h"
#include "DataFormats/Provenance/interface/FileIndex.h"
#include "DataFormats/Provenance/interface/Selections.h"
#include "IOPool/Output/src/RootOutputTree.h"
#include "DataFormats/Provenance/interface/BranchEntryInfo.h"

class TTree;
class TFile;

namespace edm {
  class PoolOutputModule;
  class History;

  class RootOutputFile {
  public:
    typedef boost::array<RootOutputTree *, NumBranchTypes> RootOutputTreePtrArray;
    explicit RootOutputFile(PoolOutputModule * om, std::string const& fileName,
                            std::string const& logicalFileName);
    ~RootOutputFile() {}
    void writeOne(EventPrincipal const& e);
    //void endFile();
    void writeLuminosityBlock(LuminosityBlockPrincipal const& lb);
    bool writeRun(RunPrincipal const& r);
    void writeBranchMapper();
    void writeEntryDescriptions();
    void writeFileFormatVersion();
    void writeFileIdentifier();
    void writeFileIndex();
    void writeEventHistory();
    void writeProcessConfigurationRegistry();
    void writeProcessHistoryRegistry();
    void writeModuleDescriptionRegistry();
    void writeParameterSetRegistry();
    void writeProductDescriptionRegistry();

    void finishEndFile();
    void beginInputFile(FileBlock const& fb, bool fastClone);
    void respondToCloseInputFile(FileBlock const& fb);

    bool isFileFull() const {return newFileAtEndOfRun_;}

  private:
    void setBranchAliases(TTree *tree, Selections const& branches) const;

  private:
    struct OutputItem {
      class Sorter {
      public:
        explicit Sorter(TTree * tree);
        bool operator() (OutputItem const& lh, OutputItem const& rh) const;
      private:
        std::map<std::string, int> treeMap_;
      };
      OutputItem() : branchDescription_(0),
	branchEntryInfoSharedPtr_(new BranchEntryInfo),
	branchEntryInfoPtr_(branchEntryInfoSharedPtr_.get()),
	selected_(false), renamed_(false), product_(0) {}
      OutputItem(BranchDescription const* bd, bool sel, bool ren) :
	branchDescription_(bd),
	branchEntryInfoSharedPtr_(new BranchEntryInfo),
	branchEntryInfoPtr_(branchEntryInfoSharedPtr_.get()),
	selected_(sel), renamed_(ren), product_(0) {}
      ~OutputItem() {}
      BranchDescription const* branchDescription_;
      boost::shared_ptr<BranchEntryInfo> branchEntryInfoSharedPtr_;
      mutable BranchEntryInfo * branchEntryInfoPtr_;
      bool selected_;
      bool renamed_;
      mutable void const* product_;
      bool operator <(OutputItem const& rh) const {
        return *branchDescription_ < *rh.branchDescription_;
      }
    };
    typedef std::vector<OutputItem> OutputItemList;
    typedef boost::array<OutputItemList, NumBranchTypes> OutputItemListArray;
    void fillItemList(Selections const& keptVector,
		      Selections const& droppedVector,
		      OutputItemList & outputItemList,
		      TTree *meta);

    void fillBranches(BranchType const& branchType, Principal const& principal) const;

    void addEntryDescription(EntryDescription const& desc);

    OutputItemListArray outputItemList_;
    std::string file_;
    std::string logicalFile_;
    JobReport::Token reportToken_;
    unsigned int eventCount_;
    unsigned int fileSizeCheckEvent_;
    PoolOutputModule const* om_;
    bool currentlyFastCloning_;
    boost::shared_ptr<TFile> filePtr_;
    FileID fid_;
    FileIndex fileIndex_;
    FileIndex::EntryNumber_t eventEntryNumber_;
    FileIndex::EntryNumber_t lumiEntryNumber_;
    FileIndex::EntryNumber_t runEntryNumber_;
    TTree * metaDataTree_;
    TTree * branchMapperTree_;
    TTree * entryDescriptionTree_;
    TTree * eventHistoryTree_;
    EventAuxiliary const*           pEventAux_;
    LuminosityBlockAuxiliary const* pLumiAux_;
    RunAuxiliary const*             pRunAux_;
    BranchEntryInfoVector const*    pBranchEntryInfoVector_;
    History const*                  pHistory_;
    RootOutputTree eventTree_;
    RootOutputTree lumiTree_;
    RootOutputTree runTree_;
    RootOutputTreePtrArray treePointers_;
    mutable bool newFileAtEndOfRun_;
    bool dataTypeReported_;
  };
}

#endif
