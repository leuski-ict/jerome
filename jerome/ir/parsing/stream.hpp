//
//  stream.hpp
//
//  Created by Anton Leuski on 9/21/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef stream_hpp
#define stream_hpp

namespace jerome { namespace stream {
  template <class Derived, typename Value>
  struct stream {
    typedef Value value_type;
    optional<value_type> next() {
      return static_cast<Derived*>(this)->get_next();
    }
  };

  template <class Predicate, class Stream>
  struct filtered_stream : public stream<
    filtered_stream<Predicate, Stream>,
    typename Stream::value_type
  >
  {
    typedef stream<
      filtered_stream<Predicate, Stream>,
      typename Stream::value_type
    > parent_type;
    typedef typename parent_type::value_type value_type;

    filtered_stream(Stream s, Predicate p)
    : mStream(s)
    , mPredicate(p)
    {}
  private:
    friend parent_type;
    Predicate mPredicate;
    Stream mStream;
    optional<value_type> get_next() {
      while (true) {
        auto x = mStream.next();
        if (!x) return x;
        if (mPredicate(*x)) return x;
      }
    }
  };

  template <class Function, class Stream>
  struct transformed_stream : public stream<
    transformed_stream<Function, Stream>,
    typename Function::result_type
  >
  {
    typedef stream<
      transformed_stream<Function, Stream>,
      typename Function::result_type
    > parent_type;
    typedef typename parent_type::value_type value_type;

    transformed_stream(Function p, Stream s)
    : mStream(s)
    , mFunction(p)
    {}
  private:
    friend parent_type;
    Function mFunction;
    Stream mStream;
    optional<value_type> get_next() {
      auto x = mStream.next();
      if (!x) return x;
      return mFunction(*x);
    }
  };

  template <typename Value>
  struct pipe_stream : public stream<
    pipe_stream<Value>,
    Value
  >
  {
    typedef stream<
      pipe_stream<Value>,
      Value
    > parent_type;
    typedef typename parent_type::value_type value_type;

  private:
    struct InputBase {
      virtual ~InputBase() {}
      virtual optional<value_type> get_next() = 0;
    };

    template <class Stream>
    struct Input : public InputBase {
      Stream mStream;
      Input(Stream inStream)
      : mStream(inStream)
      {}

      optional<value_type> get_next() override {
        return mStream.next();
      }
    };

    typedef std::unique_ptr<InputBase> input_type;

  public:
    template <class Stream>
    void setInput(Stream&& inStream) {
      mInput = input_type(new Stream(std::forward<Stream>(inStream)));
    }
  private:
    friend parent_type;
    input_type mInput;
  };

  namespace stream_detail {
    template< class T >
    struct holder
    {
      T val;
      holder( T t ) : val(t)
      { }
    };

    template< class T >
    struct holder2
    {
      T val1, val2;
      holder2( T t, T u ) : val1(t), val2(u)
      { }
    };

    template< template<class> class Holder >
    struct forwarder
    {
      template< class T >
      Holder<T> operator()( T t ) const
      {
        return Holder<T>(t);
      }
    };

    template< template<class> class Holder >
    struct forwarder2
    {
      template< class T >
      Holder<T> operator()( T t, T u ) const
      {
        return Holder<T>(t,u);
      }
    };

    template< template<class,class> class Holder >
    struct forwarder2TU
    {
      template< class T, class U >
      Holder<T, U> operator()( T t, U u ) const
      {
        return Holder<T, U>(t, u);
      }
    };
  }

  namespace stream_detail {
    template< class T >
    struct filter_holder : holder<T>
    {
      filter_holder( T r ) : holder<T>(r)
      { }
    };
    template< class T >
    struct transform_holder : holder<T>
    {
      transform_holder( T r ) : holder<T>(r)
      { }
    };
  }

  const auto filtered = stream_detail::forwarder<stream_detail::filter_holder>();
  const auto transformed = stream_detail::forwarder<stream_detail::transform_holder>();
}}

namespace jerome {
  template <class Stream, class Predicate>
  inline stream::filtered_stream<Predicate, Stream>
  operator|(Stream&& r,
            const stream::stream_detail::filter_holder<Predicate>& f)
  {
    return stream::filtered_stream<Predicate, Stream>(std::forward<Stream>(r), f.val);
  }

  template <class Stream, class Predicate>
  inline stream::transformed_stream<Predicate, Stream>
  operator|(Stream&& r,
            const stream::stream_detail::transform_holder<Predicate>& f)
  {
    return stream::transformed_stream<Predicate, Stream>(f.val, std::forward<Stream>(r));
  }
}

#endif // defined stream_hpp
