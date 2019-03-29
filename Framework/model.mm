//
//  model.m
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
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

#include <jerome/npc/model.hpp>
#import "ObjectiveC++.h"
#import "model_private.h"

using namespace jerome;

@interface ALUtterance ()
@property (nonatomic, assign) npc::Utterance utterance;
@property (nonatomic, copy, readonly, nonnull) NSArray* fields;
@property (nonatomic, copy, readonly, nonnull) NSDictionary* dictionaryWithFieldValues;
@end

@implementation ALUtterance

- (BOOL)hasValueForKey:(NSString* _Nonnull)fieldName
{
  return self->_utterance.has(fieldName.cppString);
}

- (NSString* _Nullable)valueForKey:(NSString* _Nonnull)fieldName
{
  auto name = fieldName.cppString;
  return self->_utterance.has(name)
    ? [NSString stringWithCPPString:self->_utterance.get(name)]
    : nil;
}

- (void)setValue:(NSString* _Nullable)fieldValue forKey:(NSString* _Nonnull)fieldName
{
  self->_utterance.put(fieldName.cppString, fieldValue.cppString);
}

- (NSString* _Nullable)objectForKeyedSubscript:(NSString* _Nonnull)key
{
  return [self valueForKey:key];
}

- (void)setObject:(NSString* _Nullable)obj forKeyedSubscript:(NSString* _Nonnull)key
{
  [self setValue:obj forKey:key];
}

- (NSString*)description
{
  return self.dictionaryWithFieldValues.description;
}

- (NSArray*)fields
{
  NSMutableArray* array = [NSMutableArray new];
  for(const auto& f : self->_utterance.fieldNames()) {
    [array addObject:[NSString stringWithCPPString:f]];
  }
  return array;
}

- (NSDictionary*)dictionaryWithFieldValues
{
  NSMutableDictionary* dict = [NSMutableDictionary new];
  for(NSString* f in self.fields) {
    dict[f] = [self valueForKey:f];
  }
  return dict;
}

@end

@implementation ALUtterance (AL)

+ (ALUtterance* _Nonnull)utteranceWithUtterance:(const npc::Utterance&)utterance
{
  ALUtterance* utt = [ALUtterance new];
  utt.utterance = utterance;
  return utt;
}

+ (ALUtterance* _Nullable)utteranceWithOptionalUtterance:(const optional<npc::Utterance>&)utterance
{
  return utterance ? [self utteranceWithUtterance:*utterance] : nil;
}

@end

@implementation ALPlatformEvent
- (NSString*)description
{
  return [NSString stringWithFormat:@"PlatformEvent{name=\"%@\", type=\"%@\", "
          "target=\"%@\", origin=\"%@\", data=%@}", self.name, self.type,
          self.target, self.origin, self.data];
}
@end
