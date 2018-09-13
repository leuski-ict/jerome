//
//  parsing_pipes.hpp
//
//  Created by Anton Leuski on 4/15/12.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Jerome is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Jerome is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with Jerome.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __jerome_ir_parsing_parsing_pipes_hpp__
#define __jerome_ir_parsing_parsing_pipes_hpp__

//#include "parsing.h"

namespace jerome { namespace ir {

	namespace keyword {
		BOOST_PARAMETER_NAME(dictionary)
		BOOST_PARAMETER_NAME(locale)
		BOOST_PARAMETER_NAME(field)
		BOOST_PARAMETER_NAME(unigram_field)
		BOOST_PARAMETER_NAME(bigram_field)
		BOOST_PARAMETER_NAME(stem)
		BOOST_PARAMETER_NAME(stopwords)
		BOOST_PARAMETER_NAME(string)
    BOOST_PARAMETER_NAME(index)
	}

	using namespace jerome::ir::filter;

	namespace impl {

		template <class Index>
		class TokenPipe : public TokenFilter {
			template <class ArgumentPack>
			static TokenStream make_stream(ArgumentPack const& args) {
			
				using namespace jerome::ir::keyword;

				TokenStream	stream(new Tokenizer(args[_string],
                                         args[_locale | jerome::Locale()]));
				stream	= new Lowercase(stream);
				stream	= new Apostrophe(stream);
        stream  = new Alphanumeric(stream);

				String dictionaryPath				=
        args[_dictionary | jerome::ir::filter::Dictionary::defaultDictionaryName()];
				if (dictionaryPath.length())
					stream 	= new jerome::ir::filter::Dictionary(stream, dictionaryPath);
				
				if (args[_stem | true]) 
					stream	= new KStem(stream);
					
				const Stopper::Stopwords& stopwords =
          args[_stopwords | Stopper::defaultStopwords()];
        stream	= new Stopper(stream, stopwords);
					
        Index* index  =
          args[_index | ((Index*)nullptr)];

        String*		unigrams =
          args[_unigram_field | ((String*)nullptr)];
				if (index && unigrams)
					stream	= new IndexWriter<Index>(stream, *index, *unigrams);

				String*		bigrams =
          args[_bigram_field | ((String*)nullptr)];
				if (index && bigrams) {
					stream	= new NGram(stream);
					stream	= new IndexWriter<Index>(stream, *index, *bigrams);
				}

				return stream;
			}	
		public:
			template <class ArgumentPack>
			TokenPipe(ArgumentPack const& args)
      : TokenFilter(make_stream(args))
      {}
		};
	
	}
	
  template <class Index>
  class UniversalTokenPipe : public impl::TokenPipe<Index> {
  public:
    BOOST_PARAMETER_CONSTRUCTOR(
      UniversalTokenPipe
      , (impl::TokenPipe<Index>)
      , keyword::tag
      ,
      (required
       (string,*)
       )
      (optional
       (locale,     (jerome::Locale))//,      (jerome::Locale()) )
       //,         ((Index*)NULL) )
       (index,  (Index*))
       (unigram_field,  (String*))//,         ((String*)NULL) )
       (bigram_field,  (String*))//,         ((String*)NULL) )
       (stem,      (bool))//,         (true) )
       (stopwords,    (Stopper::Stopwords*))//,  ((Stopper::Stopword*)NULL) )
       )
    )
  };

	namespace impl {

		template <class Index>
		class NonTokenizingPipe : public TokenFilter {
			template <class ArgumentPack>
			static TokenStream make_stream(ArgumentPack const& args) {
			
				using namespace jerome::ir::keyword;
				TokenStream	stream(new NonTokenizer(args[_string],
                                            args[_locale | jerome::Locale()]));
			  String* fieldName	=
          args[_field | ((String*)nullptr)];
        Index* index  =
          args[_index | ((Index*)nullptr)];
				if (fieldName && index)
					stream	= new IndexWriter<Index>(stream, *index, *fieldName);
				return stream;
			}	
		public:
			template <class ArgumentPack>
			NonTokenizingPipe(ArgumentPack const& args)
      : TokenFilter(make_stream(args))
      {}
		};		
	}
	
  template <class Index>
  class NonTokenizingPipe : public impl::NonTokenizingPipe<Index> {
  public:
    BOOST_PARAMETER_CONSTRUCTOR(
      NonTokenizingPipe
      , (impl::NonTokenizingPipe<Index>)
      , keyword::tag
      ,
      (required
       (string,*)
       )
      (optional
       (locale,     (jerome::Locale))//,      (jerome::Locale()) )
       (index,  (Index*))
       (field,      (String*))//,         ((String*)NULL) )
       )
    )
  };

}}



#endif // defined __jerome_ir_parsing_parsing_pipes_hpp__
