/*
 * InputPath.cpp
 *
 *  Created on: 23 Oct 2015
 *      Author: hieu
 */
#include <boost/foreach.hpp>
#include "InputPath.h"
#include "TargetPhrases.h"
#include "ActiveChart.h"
#include "../TranslationModel/PhraseTable.h"
#include "../MemPool.h"

using namespace std;

namespace Moses2
{
namespace SCFG
{

InputPath::InputPath(MemPool &pool, const SubPhrase<SCFG::Word> &subPhrase,
    const Range &range, size_t numPt, const InputPath *prefixPath)
:InputPathBase(pool, range, numPt, prefixPath)
,subPhrase(subPhrase)
{
  MemPoolAllocator< std::pair<SymbolBind, SCFG::TargetPhrases> > collAlloc(pool);
  targetPhrases = new (pool.Allocate<Coll>()) Coll(collAlloc);
  m_activeChart = pool.Allocate<ActiveChart>(numPt);
  for (size_t i = 0; i < numPt; ++i) {
    ActiveChart &memAddr = m_activeChart[i];
    new (&memAddr) ActiveChart(pool);
  }
}

InputPath::~InputPath()
{
  // TODO Auto-generated destructor stub
}

void InputPath::Debug(std::ostream &out, const System &system) const
{
  out << range << " ";
  subPhrase.Debug(out, system);
  out << " " << prefixPath << " ";

  const Vector<ActiveChartEntry*> &activeEntries = *GetActiveChart(1).entries;
  out << "m_activeChart=" << activeEntries.size() << " ";

  for (size_t i = 0; i < activeEntries.size(); ++i) {
    const ActiveChartEntry &entry = *activeEntries[i];
    entry.GetSymbolBind().Debug(out, system);
    out << "| ";
  }

  // tps
  out << "tps=" << targetPhrases->size();

  out << " ";
  BOOST_FOREACH(const SCFG::InputPath::Coll::value_type &valPair, *targetPhrases) {
    const SymbolBind &symbolBind = valPair.first;
    const SCFG::TargetPhrases &tps = *valPair.second;
    symbolBind.Debug(out, system);
    //out << "=" << tps.GetSize() << " ";
    tps.Debug(out, system);
  }
}

void InputPath::AddTargetPhrase(
    MemPool &pool,
    const PhraseTable &pt,
    const SCFG::SymbolBind &symbolBind,
    const SCFG::TargetPhraseImpl *tp)
{
  SCFG::TargetPhrases *tps;
  Coll::iterator iter;
  iter = targetPhrases->find(symbolBind);
  if (iter == targetPhrases->end()) {
    tps = new (pool.Allocate<SCFG::TargetPhrases>()) SCFG::TargetPhrases(pool);
    (*targetPhrases)[symbolBind] = tps;
  }
  else {
    tps = iter->second;
  }

  tps->AddTargetPhrase(*tp);
}

void InputPath::AddActiveChartEntry(size_t ptInd, ActiveChartEntry *chartEntry)
{
  //cerr << "      added " << chartEntry << " " << range << " " << ptInd << endl;
  ActiveChart &activeChart = m_activeChart[ptInd];
  activeChart.entries->push_back(chartEntry);
}

size_t InputPath::GetNumRules() const
{
  size_t ret = 0;
  BOOST_FOREACH(const Coll::value_type &valPair, *targetPhrases) {
    const SCFG::TargetPhrases &tps = *valPair.second;
    ret += tps.GetSize();
  }
  return ret;
}

}
}
