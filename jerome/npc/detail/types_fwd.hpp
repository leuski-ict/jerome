//
//  types_fwd.hpp
//
//  Created by Anton Leuski on 8/1/15.
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

#ifndef __jerome_npc_detail_types_fwd_hpp__
#define __jerome_npc_detail_types_fwd_hpp__

namespace jerome {

	template <typename> class Providable;

	
	namespace ir {

		class HeapIndex;
		template <class Q, class A, class L> class Ranker;
		
		namespace training {
			template <class Q, class A, class L> class Data;
			template <class Q, class D> class TestSet;
		}
		
	}
	
	namespace npc {
	
		class Utterance;
		class Link;
		
		namespace detail {
	
			typedef jerome::ir::training::Data <
				Utterance,
				Utterance,
				Link
			> Data;
		
			typedef jerome::ir::training::Data <
				jerome::ir::HeapIndex,
				Utterance,
				Link
			>	IndexedData;
		
			template <class Q, class A> class RankerImplementation;
			template <class Q, class A> class RankerInterface;
			
			typedef RankerInterface<
				Utterance,
				Utterance
			> Ranker;
			
			typedef RankerInterface<
				jerome::ir::HeapIndex,
				Utterance
			> IndexedRanker;

			template <class Q, class A>
			using TestSet = jerome::ir::training::TestSet<Q, A>;
			
		}
	}
}

#endif // defined __jerome_npc_detail_types_fwd_hpp__
