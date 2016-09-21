//
//  Train.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include "Train.hpp"

#include <boost/algorithm/string/join.hpp>

#include <jerome/type/algorithm.hpp>
#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>
#include <jerome/npc/factories/AnalyzerFactory.hpp>
#include <jerome/npc/factories/TrainerFactory.hpp>
#include <jerome/npc/factories/RankerFactory.hpp>
#include <jerome/npc/factories/WeightingFactory.hpp>

#include <jerome/type/Factory.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>
#include <jerome/ir/report/XMLReporter.hpp>
#include "split.private.hpp"

static const char* oInputFile           = "input";
static const char* oOutputFile          = "output";
static const char* oReportFile          = "report";
static const char* oTestSplit           = "test-split";
static const char* oDevSplit            = "dev-split";
static const char* oMaxTime             = "max-time";
static const char* oQuestionAnalyzer    = "question-analyzer";
static const char* oAnswerAnalyzer      = "answer-analyzer";
static const char* oQuestionWeighting   = "question-weighting";
static const char* oAnswerWeighting     = "answer-weighting";
static const char* oTrainer             = "trainer";
static const char* oVerbosity           = "verbosity";

using namespace jerome;
using namespace jerome::npc;

typedef TrainerFactory::object_type TR;

template <typename F>
static std::pair<std::string, std::string>
modelNames()
{
  auto models = F::sharedInstance().predefinedModels();
  auto modelNames = keys(models);
  auto modelNamesString = boost::algorithm::join(modelNames, "\n  ");
  auto defaultModelName = F::sharedInstance().defaultModelKey();
  return std::make_pair(modelNamesString, defaultModelName);
}

po::options_description Train::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  auto analyzerModels = modelNames<AnalyzerFactory>();
  auto qwModels = modelNames<QuestionWeightingFactory>();
  auto awModels = modelNames<AnswerWeightingFactory>();
  auto trainerModels = modelNames<TrainerFactory>();

  options.add_options()
  (oVerbosity, 	po::value<int>()->default_value(int(0)),
   "verbosity level")
  (oInputFile, 	po::value<std::string>()->default_value("-"),
   "input file (default: standard input)")
  (oOutputFile, po::value<std::string>(),
   "output file (default: none)")
  (oReportFile, po::value<std::string>()->default_value("-"),
   "report file")
  (oMaxTime, po::value<int>(),
   "maximum training time in seconds")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions. Provide one of \n")
    + "  auto      \t\n"
    + "  label     \t\n"
    + "  <number>  \t\n"
    + "  <number>% \t\n"
    )
   .c_str())
  (oDevSplit,   po::value<std::string>()->default_value("label"),
   (std::string("How to select development questions. Provide one of \n")
    + "  auto      \t\n"
    + "  label     \t\n"
    + "  <number>  \t\n"
    + "  <number>% \t\n"
    )
   .c_str())
  (oQuestionAnalyzer, po::value<std::string>()
    ->default_value(analyzerModels.second),
   (std::string("question analyzer. One of\n  ")
    + analyzerModels.first).c_str())
  (oAnswerAnalyzer, po::value<std::string>()
    ->default_value(analyzerModels.second),
   (std::string("answer analyzer. One of\n  ")
    + analyzerModels.first).c_str())
  (oQuestionWeighting, po::value<std::string>()
   ->default_value(qwModels.second),
   (std::string("question weighting. One of\n  ")
    + qwModels.first).c_str())
  (oAnswerWeighting, po::value<std::string>()
   ->default_value(awModels.second),
   (std::string("answer weighting. One of\n  ")
    + awModels.first).c_str())
  (oTrainer, po::value<std::string>()
   ->default_value(trainerModels.second),
   (std::string("training measure. One of\n  ")
    + trainerModels.first).c_str())
  (TR::G_ALGORITHM, po::value<std::string>()
   ->default_value("GN_DIRECT_L_RAND"),
   (std::string("global optimization algorithm. One of\n  ")
    + boost::algorithm::join(TR::globalAlgorithms(), "\n  ")).c_str())
  (TR::L_ALGORITHM, po::value<std::string>(),
   (std::string("some global optimization algorithms, i.e., ")
    + boost::algorithm::join(TR::globalAlgorithmsRequiringLocalOptimizer(), ", ")
    + " require a local optimizer. Try LN_BOBYQA first. One of\n  "
    + boost::algorithm::join(TR::localAlgorithms(), "\n  ")).c_str())
  ;
  
  po::options_description stopping("Stopping criteria");
  stopping.add_options()
  (TR::G_FTOL_REL, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "the objective function value by less than this value multiplied by the "\
   "absolute value of the function value. (If there is any chance that your "\
   "optimum function value is close to zero, you might want to set an "\
   "absolute tolerance as well.) As a first take, set this value to something "\
   "like 0.0001.")
  (TR::G_FTOL_ABS, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "the function value by less than this value.")
  (TR::G_XTOL_REL, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "every parameter by less than this value multiplied by the absolute value "\
   "of the parameter. (If there is any chance that an optimal parameter is "\
   "close to zero, you might want to set an absolute tolerance as well.)")
  (TR::G_XTOL_ABS, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "every parameter value by less than this value.")
  (TR::L_FTOL_REL, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes the objective function value by less than this value multiplied "\
   "by the absolute value of the function value. (If there is any chance that "\
   "your optimum function value is close to zero, you might want to set an "\
   "absolute tolerance as well.)")
  (TR::L_FTOL_ABS, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes the function value by less than this value.")
  (TR::L_XTOL_REL, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes every parameter by less than this value multiplied by the "\
   "absolute value of the parameter. (If there is any chance that an optimal "\
   "parameter is close to zero, you might want to set an absolute tolerance "\
   "as well.)")
  (TR::L_XTOL_ABS, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes every parameter value by less than this value.")
  ;
  
  options.add(stopping);
  
  return options;
}

// ----------------------------------------------------------------------------


template <typename T>
static void recordEmplace(Record& ioRecord,
                          const String& key, const po::variables_map& vm)
{
  if (!vm[key].empty())
    ioRecord.emplace(key, vm[key].as<T>());
}

static Result<TrainerFactory::object_type>
makeTrainer(const po::variables_map& inVM)
{
  auto trainerMeasure = inVM[oTrainer].as<std::string>();
  auto trainerModel = TrainerFactory::sharedInstance()
  .modelAt(trainerMeasure);
  if (!trainerModel) {
    return Error("No trainer measure with id " + trainerMeasure);
  }
  
  auto model = *trainerModel;
  
  recordEmplace<String>(model, TR::G_ALGORITHM, inVM);
  recordEmplace<String>(model, TR::L_ALGORITHM, inVM);
  recordEmplace<double>(model, TR::G_FTOL_REL, inVM);
  recordEmplace<double>(model, TR::G_FTOL_ABS, inVM);
  recordEmplace<double>(model, TR::G_XTOL_REL, inVM);
  recordEmplace<double>(model, TR::G_XTOL_ABS, inVM);
  recordEmplace<double>(model, TR::L_FTOL_REL, inVM);
  recordEmplace<double>(model, TR::L_FTOL_ABS, inVM);
  recordEmplace<double>(model, TR::L_XTOL_REL, inVM);
  recordEmplace<double>(model, TR::L_XTOL_ABS, inVM);

  return TrainerFactory::sharedInstance().make(model);
}

#define COPY_RECORD(F, K, A) \
{ \
  auto key = inVM[K].as<std::string>(); \
  auto optional_v = F::sharedInstance().modelAt(key); \
  if (!optional_v) { \
    return Error(String("Unknown model ") + key + " " + K); \
  } \
  model.emplace(UtteranceCLRankerModel::A, *optional_v);\
}

static
Result<Record> makeRankerModel(const po::variables_map& inVM)
{
  Record model = Record {
    RankerFactory::PROVIDER_KEY, UtteranceCLRankerModel::IDENTIFIER };

  COPY_RECORD(AnswerWeightingFactory, oAnswerWeighting, ANSWER_WEIGHTING_KEY);
  COPY_RECORD(QuestionWeightingFactory, oQuestionWeighting, QUESTION_WEIGHTING_KEY);
  COPY_RECORD(AnalyzerFactory, oAnswerAnalyzer, ANSWER_ANALYZER_KEY);
  COPY_RECORD(AnalyzerFactory, oQuestionAnalyzer, QUESTION_ANALYZER_KEY);
  
  return model;
}

// ----------------------------------------------------------------------------

OptionalError Train::run1Classifier(const std::string& classifierName)
{
  auto optState = platform().collection().states()
  .optionalObjectWithName(classifierName);
  if (!optState) {
    return Error(std::string("Invalid classifier name: ") + classifierName);
  }
  
  std::pair<UL, UL>   testTrainSplit =
  splitData<const Domain::utterances_type&>(variables(), oTestSplit,
                                            optState->questions().utterances(),
                                            0.1, "test");
  std::pair<UL, UL>   devTrainSplit =
  splitData(variables(), oDevSplit, testTrainSplit.second, 0.15, "dev");
  
  auto trainer_or_error = makeTrainer(variables());
  if (!trainer_or_error) {
    return trainer_or_error.error();
  }

  auto ranker_or_error = makeRankerModel(variables());
  if (!ranker_or_error) {
    return ranker_or_error.error();
  }

  int maxTime = 0;
  if (!variables()[oMaxTime].empty())
    maxTime = variables()[oMaxTime].as<int>();
  
  double bestValue = 0;
  int verbosity = variables()[oVerbosity].as<int>();
  
  TrainingParameters<Utterance> params;

  params.stateName = classifierName;
  params.callback = [&bestValue,maxTime,verbosity](TrainingState& state) {
    if (maxTime > 0 && state.elapsedTimeInSeconds() > maxTime)
      state.stop();
    if (state.lastValue() > bestValue) {
      bestValue = state.lastValue();
      if (verbosity > 2) {
        std::cout
          << state.name() << ": " << state.lastArgument()
          << " " << bestValue << std::endl;
      }
    }
  };
  params.developmentQuestions = devTrainSplit.first;
  params.trainingQuestions = devTrainSplit.second;
  params.trainer = trainer_or_error.value();
  params.rankerModel = ranker_or_error.value();
  
  EvaluationParameters<Utterance> eparams;
  
  Record eargs(jerome::detail::FactoryConst::PROVIDER_KEY,
              jerome::ir::evaluation::detail::HTMLReporterBase::IDENTIFIER);
  
  eparams.stateName = classifierName;
  eparams.testQuestions = testTrainSplit.first;
  eparams.trainingQuestions = testTrainSplit.second;
  eparams.report = Command::nullOStream();
  eparams.reporterModel = eargs;
  
  auto acc_or_error_before = platform().evaluate(eparams);
  if (!acc_or_error_before) return acc_or_error_before.error();
  
  auto error = platform().train(params);
  if (error) return error;

  auto acc_or_error_after = platform().evaluate(eparams);
  if (!acc_or_error_after) return acc_or_error_after.error();

  if (verbosity > 1) {
    std::cout << acc_or_error_before.value() << " -> ";
  }
  if (verbosity > 0) {
    std::cout << acc_or_error_after.value() << std::endl;
  }

  return Error::NO_ERROR;
}

OptionalError Train::setup()
{
  return platform().loadCollection(*istreamWithName(variables()[oInputFile]));
}

OptionalError Train::teardown()
{
  return platform().saveCollection(*ostreamWithName(variables()[oOutputFile]));
}

std::string Train::description() const
{
  return "train a classifier";
}

std::string Train::name() const
{
  return "train";
}


