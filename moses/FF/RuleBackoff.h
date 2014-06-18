#pragma once

#include <string>
#include "StatelessFeatureFunction.h"

namespace Moses
{

class RuleBackoff : public StatelessFeatureFunction
{
public:
  RuleBackoff(const std::string &line);

  bool IsUseable(const FactorMask &mask) const {
    return true;
  }

  void Evaluate(const Phrase &source
                , const TargetPhrase &targetPhrase
                , ScoreComponentCollection &scoreBreakdown
                , ScoreComponentCollection &estimatedFutureScore) const;
  void Evaluate(const InputType &input
                , const InputPath &inputPath
                , const TargetPhrase &targetPhrase
                , const StackVec *stackVec
                , ScoreComponentCollection &scoreBreakdown
                , ScoreComponentCollection *estimatedFutureScore = NULL) const;
  void Evaluate(const Hypothesis& hypo,
                ScoreComponentCollection* accumulator) const;
  void EvaluateChart(const ChartHypothesis &hypo,
                     ScoreComponentCollection* accumulator) const;
  void Evaluate(ChartTranslationOptionList &transOptList) const;

  void SetParameter(const std::string& key, const std::string& value);

  std::vector<float> DefaultWeights() const;

protected:
  float m_minCount;
};

}

