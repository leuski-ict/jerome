//
//  token.hpp
//
//  Created by Anton Leuski on 11/1/14.
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

#ifndef __jerome_ir_parsing_token_hpp__
#define __jerome_ir_parsing_token_hpp__

namespace jerome {
	
	class Locale;
	
	namespace ir {
		
		// -----------------------------------------------------------------------------

    namespace detail {
      struct TokenBase {
        typedef uint32_t  size_type;

        TokenBase(size_type inOffset = 0,
                  size_type inLength = 0,
                  uint32_t inType = 0)
        : mOffset(inOffset)
        , mLength(inLength)
        , mType(inType)
        {}

        TokenBase(TokenBase&&) = default;
        TokenBase(const TokenBase&) = default;
        TokenBase& operator = (TokenBase&& inSource) = default;
        TokenBase& operator = (const TokenBase& inSource) = default;

        size_type    offset()   const { return mOffset; }
        size_type    length()   const { return mLength; }
        size_type    end()    const { return offset() + length(); }
        uint32_t    type()     const { return mType; }

        size_type&    offset()   { return mOffset; }
        size_type&    length()   { return mLength; }
        uint32_t&    type()     { return mType; }
      private:
        size_type    mOffset;
        size_type    mLength;
        uint32_t    mType;
      };
    }

    template <class S = String>
    class BasicToken : public detail::TokenBase {
      typedef S value_type;
      typedef detail::TokenBase parent_type;

		private:
			value_type			mText;

		public:
      using parent_type::parent_type;

			BasicToken(const value_type& inText = value_type(),
                 size_type inOffset = 0,
                 size_type inLength = 0,
                 uint32_t inType = 0)
      : parent_type(inOffset, inLength, inType)
      , mText(inText)
      {}
			
			BasicToken(const String::value_type* inText,
                 size_type inOffset,
                 size_type inLength,
                 uint32_t inType = 0)
      : parent_type(inOffset, inLength, inType)
			, mText(inText+inOffset, inLength)
      {}
			
			BasicToken(const String::value_type* inText,
                 size_type inTextLength,
                 size_type inOffset,
                 size_type inLength,
                 uint32_t inType = 0)
      : parent_type(inOffset, inLength, inType)
      ,  mText(inText, inTextLength)
      {}

			const String&	text() 		const { return mText; }
			String&			text()		{ return mText; }

      BasicToken& operator += (const BasicToken& inToken) {
        if (text().size() > 0) {
          text() += ngramSeparator();
        } else {
          type() = inToken.type();
        }
        text()    += inToken.text();
        size_type  finish  = std::max(end(), inToken.end());
        offset()  = std::min(offset(), inToken.offset());
        length()  = finish - offset();
        return *this;
      }

      static value_type ngramSeparator() {
        static const value_type separator = "_";
        return separator;
      }
		};

    typedef BasicToken<String>  Token;
		
		// -----------------------------------------------------------------------------
		
		struct TokenStream;
		
		namespace i {
			
			struct TokenStreamImpl {
				virtual ~TokenStreamImpl() {}
				virtual bool getNextToken(Token& ioToken) = 0;
				virtual const jerome::Locale& locale() const { return kDefaultLocale; }
				void run() {
					Token tok;
					while (getNextToken(tok));
				}
			private:
        // STATIC
				static const jerome::Locale	kDefaultLocale;
			};
			
		}
		
		struct TokenStream : public ReferenceClassInterface<i::TokenStreamImpl> {
			typedef ReferenceClassInterface<i::TokenStreamImpl> parent_type;
			TokenStream() = default;
			TokenStream(i::TokenStreamImpl* inSource) : parent_type(shared_ptr<implementation_type>(inSource)) {}
			bool getNextToken(Token& ioToken) { return implementation().getNextToken(ioToken); }
			const jerome::Locale& locale() const { return implementation().locale(); }
			void run() { implementation().run(); }
		};
		
		// -----------------------------------------------------------------------------
		
		class TokenFilter : public i::TokenStreamImpl {
			jerome::ir::TokenStream		mSource;
		public:
			explicit TokenFilter(jerome::ir::TokenStream inSource) : mSource(inSource) {};
			jerome::ir::TokenStream& source() { return mSource; }
			const jerome::ir::TokenStream& source() const { return mSource; }
			bool getNextToken(Token& ioToken) { return source().getNextToken(ioToken); }
			const jerome::Locale& locale() const { return source().locale(); }
		};
	
	}
}

#endif // defined __jerome_ir_parsing_token_hpp__
