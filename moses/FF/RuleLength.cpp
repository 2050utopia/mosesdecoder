#include <boost/shared_ptr.hpp>
#include "RuleLength.h"
#include "moses/StaticData.h"
#include "moses/Word.h"
#include "moses/ChartCellLabel.h"
#include "moses/WordsRange.h"
#include "moses/StackVec.h"
#include "moses/TargetPhrase.h"
#include "moses/PP/PhraseProperty.h"
#include "moses/PP/RuleLengthPhraseProperty.h"

using namespace std;

namespace Moses
{
RuleLength::RuleLength(const std::string &line)
:StatelessFeatureFunction(1, line)
,m_smoothingMethod(PlusConst)
,m_const(1)
{
  m_requireSortingAfterSourceContext = true;
  ReadParameters();
}

void RuleLength::EvaluateInIsolation(const Phrase &source
						, const TargetPhrase &targetPhrase
						, ScoreComponentCollection &scoreBreakdown
						, ScoreComponentCollection &estimatedFutureScore) const
{
}

void RuleLength::EvaluateWithSourceContext(const InputType &input
                                   , const InputPath &inputPath
                                   , const TargetPhrase &targetPhrase
                                   , const StackVec *stackVec
                                   , ScoreComponentCollection &scoreBreakdown
                                   , ScoreComponentCollection *estimatedFutureScore) const
{
  assert(stackVec);

  const PhraseProperty *property = targetPhrase.GetProperty("RuleLength");
  if (property == NULL) {
	  return;
  }

  const RuleLengthPhraseProperty *slProp = static_cast<const RuleLengthPhraseProperty*>(property);

  float score = 0;
  for (size_t i = 0; i < stackVec->size(); ++i) {
	  const ChartCellLabel &cell = *stackVec->at(i);
	  const WordsRange &ntRange = cell.GetCoverage();
	  size_t sourceWidth = ntRange.GetNumWordsCovered();
	  float prob = slProp->GetProb(i, sourceWidth, m_const);
	  score += TransformScore(prob);
  }

  if (score < -100.0f) {
    float weight = StaticData::Instance().GetWeight(this);
    if (weight < 0) {
    	score = -100;
    }
  }

  scoreBreakdown.PlusEquals(this, score);

}

void RuleLength::SetParameter(const std::string& key, const std::string& value)
{
  if (key == "smoothing") {
	  if (value == "plus-constant") {
		  m_smoothingMethod = PlusConst;
	  }
	  else if (value == "none") {
		  m_smoothingMethod = None;
	  }
	  else {
		  UTIL_THROW(util::Exception, "Unknown smoothing type " << value);
	  }
  }
  else if (key == "constant") {
	  m_const = Scan<float>(value);
  }
  else {
    StatelessFeatureFunction::SetParameter(key, value);
  }
}

}

