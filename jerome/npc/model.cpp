//
//  model.cpp
//
//  Created by Anton Leuski on 1/25/12.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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
#include <jerome/npc/detail/model_impl.hpp>
#include <jerome/type/regex.hpp>

namespace jerome {
  namespace npc {

#pragma mark - ModelObject

    ModelObject::operator bool () const
    {
      return mImplementation.get() != nullptr;
    }

    std::size_t ModelObject::hash() const
    {
      return implementation().hash();
    }

    bool ModelObject::equal_to(const ModelObject& x) const
    {
      return implementation().equal_to(x.implementation());
    }

    ModelObject::ModelObject(const shared_ptr<impl_type>& inImplementation)
      : mImplementation(inImplementation)
    {
    }

    ModelObject::impl_type&       ModelObject::implementation()
    {
      return *mImplementation;
    }
    const ModelObject::impl_type& ModelObject::implementation() const
    {
      return *mImplementation;
    }

#pragma mark - Utterance

    Utterance::Utterance(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    String  Utterance::get(const String& key) const
    {
      return implementation().get(key);
    }

    String  Utterance::get(const String& key, const String& inDefault) const
    {
      return implementation().get(key, inDefault);
    }

    bool  Utterance::has(const String& key) const
    {
      return implementation().has(key);
    }

    void  Utterance::put(const String& key, const String& value)
    {
      implementation().put(key, value);
    }

    void  Utterance::remove(const String& key)
    {
      implementation().remove(key);
    }

    const Utterance::field_names_type&   Utterance::fieldNames() const
    {
      return implementation().fieldNames();
    }

    Utterance::impl_type&   Utterance::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const Utterance::impl_type& Utterance::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

    double Utterance::count() const
    {
      return implementation().count();
    }
    void Utterance::setCount(double count)
    {
      implementation().setCount(count);
    }

#pragma mark - Link

    Link::Link(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    Link::utterances_type&    Link::utterances()
    {
      return implementation().utterances();
    }

    const Link::utterances_type&  Link::utterances() const
    {
      return implementation().utterances();
    }

    Link::value_type Link::value() const
    {
      return implementation().value();
    }

    void Link::setValue(value_type inValue)
    {
      return implementation().setValue(inValue);
    }

    Link::impl_type&   Link::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const Link::impl_type& Link::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - NamedModelObject

    NamedModelObject::NamedModelObject(
      const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    String NamedModelObject::name() const
    {
      return implementation().name();
    }

    void NamedModelObject::setName(const String& inName)
    {
      implementation().setName(inName);
    }

    NamedModelObject::impl_type&   NamedModelObject::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const NamedModelObject::impl_type& NamedModelObject::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - Domain

    Domain::Domain(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    Domain::utterances_type&  Domain::utterances()
    {
      return implementation().utterances();
    }

    const Domain::utterances_type&  Domain::utterances() const
    {
      return implementation().utterances();
    }

    Domain::impl_type&   Domain::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const Domain::impl_type& Domain::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - Lattice

    Lattice::Lattice(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    Lattice::links_type& Lattice::links()
    {
      return implementation().links();
    }

    const Lattice::links_type& Lattice::links() const
    {
      return implementation().links();
    }

    Lattice::impl_type&   Lattice::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const Lattice::impl_type& Lattice::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - State

    State::State(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    Domain   State::answers() const
    {
      return implementation().answers();
    }

    void State::setAnswers(const Domain&  inAnswers)
    {
      implementation().setAnswers(inAnswers);
    }

    Domain   State::questions() const
    {
      return implementation().questions();
    }

    void State::setQuestions(const Domain& inQuestions)
    {
      implementation().setQuestions(inQuestions);
    }

    Lattice  State::links() const
    {
      return implementation().links();
    }

    void State::setLinks(const Lattice& inLinks)
    {
      implementation().setLinks(inLinks);
    }

    State::ranker_model_type  State::rankerModel() const
    {
      return implementation().rankerModel();
    }

    void State::setRankerModel(const State::ranker_model_type& inClassifier)
    {
      implementation().setRankerModel(inClassifier);
    }

    State::impl_type&   State::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const State::impl_type& State::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - Collection

    Collection::Collection(const shared_ptr<impl_type>& inImplementation)
      : parent_type(inImplementation)
    {
    }

    Collection::domains_type&     Collection::domains()
    {
      return implementation().domains();
    }

    const Collection::domains_type&   Collection::domains() const
    {
      return implementation().domains();
    }

    Collection::states_type&      Collection::states()
    {
      return implementation().states();
    }

    const Collection::states_type&    Collection::states() const
    {
      return implementation().states();
    }

    Collection::lattices_type&      Collection::lattices()
    {
      return implementation().lattices();
    }

    const Collection::lattices_type&  Collection::lattices() const
    {
      return implementation().lattices();
    }

    Collection::links_type&       Collection::links()
    {
      return implementation().links();
    }

    const Collection::links_type&   Collection::links() const
    {
      return implementation().links();
    }

    Collection::utterances_type&    Collection::utterances()
    {
      return implementation().utterances();
    }

    const Collection::utterances_type&  Collection::utterances() const
    {
      return implementation().utterances();
    }

    Collection::utterance_index_type  Collection::utterance_index() const
    {
      return implementation().utterance_index();
    }

    Collection::impl_type&   Collection::implementation()
    {
      return static_cast<impl_type&>(parent_type::implementation());
    }

    const Collection::impl_type& Collection::implementation() const
    {
      return static_cast<const impl_type&>(parent_type::implementation());
    }

#pragma mark - Collection::utterance_index_type

    Collection::utterance_index_type::utterance_index_type(const shared_ptr<
        impl_type>& inImplementation)
      : mImplementation(inImplementation)
    {
    }

    String Collection::utterance_index_type::expandFieldValue(
      const String& field,
      const String& inValue) const
    {
      // STATIC
      static auto kImportPattern = new rgx::regex("<import\\s+(.*?)\\s*=\\s*\"(.*?)\".*?>",
        rgx::regex_constants::optimize
        | rgx::regex_constants::icase);
      static auto kWhitespacePrefixPattern = new rgx::regex("^\\s.*",
        rgx::regex_constants::optimize
        | rgx::regex_constants::icase);
      static auto kWhitespaceSuffixPattern = new rgx::regex(".*\\s$",
        rgx::regex_constants::optimize
        | rgx::regex_constants::icase);

      rgx::smatch match;
      if (!rgx::regex_search(inValue, match,
            *kImportPattern) || match.empty()) return inValue;

      String    text;

      while (true) {

        auto  replacementUtterance  =
          utteranceWithFieldValue(match[1], match[2]);

        String    replacement;

        if (replacementUtterance) {
          replacement = replacementUtterance->get(field, replacement);
        }

        String    delim1, delim2;

        if (match.prefix().length()
           && replacement.length()
           && !rgx::regex_match(match.prefix().str(),
              *kWhitespaceSuffixPattern)
           && !rgx::regex_match(replacement,
              *kWhitespacePrefixPattern)) delim1 = " ";

        if (match.suffix().length()
           && replacement.length()
           && !rgx::regex_match(match.suffix().str(),
              *kWhitespacePrefixPattern)
           && !rgx::regex_match(replacement,
              *kWhitespaceSuffixPattern)) delim2 = " ";

        text = match.prefix().str() + delim1 + replacement + delim2 +
               match.suffix().str();

        if (!rgx::regex_search(text, match,
              *kImportPattern) || match.empty()) return text;
      }
    }

    OptionalUtterance Collection::utterance_index_type::utteranceWithFieldValue(
      const String& field,
      const String& value) const
    {
      return mImplementation->utteranceWithFieldValue(field, value);
    }

    OptionalUtterance Collection::utterance_index_type::utteranceWithFieldValue(
      const String& field,
      const String& value)
    {
      return mImplementation->utteranceWithFieldValue(field, value);
    }

    Collection::utterance_index_type::operator bool () const
    {
      return mImplementation.get() != nullptr;
    }

#pragma mark - ObjectFactory

    ObjectFactory::~ObjectFactory()
    {
    }

    namespace detail {
      struct ExpandingUtterance
        : detail::UtteranceEnvelope
      {
        typedef detail::UtteranceEnvelope parent_type;

        ExpandingUtterance(const Utterance& inSource,
          const Collection::utterance_index_type& inIndex)
          : parent_type(inSource)
          , mIndex(inIndex)
        {
        }

        String get(const String& key) const
        {
          return mIndex.expandFieldValue(key, source().get(key));
        }

        String get(const String& key, const String& inDefault) const
        {
          return mIndex.expandFieldValue(key, source().get(key, inDefault));
        }

        static Utterance envelop(const Utterance& inSource,
                                 const Collection::utterance_index_type& inIndex)
        {
          return parent_type::envelop<ExpandingUtterance>(inSource, inIndex);
        }

      private:
        Collection::utterance_index_type  mIndex;
      };
    }

    FieldValueExpander::FieldValueExpander(
      const Collection::utterance_index_type& inIndex)
      : mIndex(inIndex)
    {
    }

    Utterance FieldValueExpander::operator () (const Utterance& inUtterance)
    const
    {
      return detail::ExpandingUtterance::envelop(inUtterance, mIndex);
    }

  }
}
