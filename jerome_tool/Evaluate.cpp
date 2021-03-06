//
//  Evaluate.cpp
//  jerome
//
//  Created by Anton Leuski on 9/9/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <fstream>

#include "Evaluate.hpp"

#include <jerome/type/algorithm.hpp>
#include <jerome/type/Factory.hpp>
#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelReader.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>
#include <jerome/ir/report/XMLReporter.hpp>

static const char* oReportFile	    = "report";
static const char* oReportFormat    = "report-format";
static const char* oTestSplit       = "test-split";

#include "split.private.hpp"

using namespace jerome;
using namespace jerome::npc;

static SplitMenu<const Domain::utterances_type&> testTrainSplitActions("test", 0.1);

po::options_description Evaluate::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  appendInputOptions(options);
  
  const char* CHOICE = "\nProvide one of";

  options.add_options()
  (oReportFile, po::value<std::string>(),
   "report file name format string (default: none), e.g., "\
   "\"report-%s-%s.xml\". The file will be named by replacing the first " \
   "argument in the format with input file name and the second with " \
   "the classifier name." )
  (oReportFormat, po::value<std::string>()->default_value("html"),
   "report file format. One of 'xml' or 'html'.")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions.")
    + CHOICE + testTrainSplitActions.description() + "\n"
    )
   .c_str())
  ;
  
  return options;
}

OptionalError Evaluate::setup()
{
  return loadCollection();
}

OptionalError Evaluate::teardown()
{
  return Error::NO_ERROR;
}

OptionalError Evaluate::run1Classifier(const std::string& classifierName)
{
  auto optState = platform().collection().states()
    .optionalObjectWithName(classifierName);

  if (!optState) {
    return Error(std::string("Invalid classifier name: ") + classifierName);
  }

  auto format_or_error = parseFormat(variables());
  if (!format_or_error) return format_or_error.error();
  
  auto   testTrainSplit = testTrainSplitActions
    .split(optState->questions().utterances(),
           variables()[oTestSplit].as<String>());
  if (!testTrainSplit) {
    return testTrainSplit.error();
  }
  
  Record args(jerome::detail::FactoryConst::PROVIDER_KEY,
              format_or_error.value());

  EvaluationParameters<Utterance> params;
  
  params.stateName = classifierName;
  params.testQuestions = testTrainSplit.value().first;
  params.trainingQuestions = testTrainSplit.value().second;
  params.report = parseReportStream(classifierName, variables(), 
                                    inputFileName(variables()));
  params.reporterModel = args;
  
  auto acc_or_error = platform().evaluate(params);
  
  if (!acc_or_error) {
    return acc_or_error.error();
  }
  
  std::cout << acc_or_error.value() << std::endl;
  
  return Error::NO_ERROR;
}

std::string Evaluate::description() const
{
  return "evaluate a classifier";
}

std::string Evaluate::name() const
{
  return "evaluate";
}

