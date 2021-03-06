//
//  distances_in_mixed_space.hpp
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __jerome_ir_rm_distances_in_mixed_space_hpp__
#define __jerome_ir_rm_distances_in_mixed_space_hpp__

#include <jerome/ir/distances.hpp>

// this is a weird formula: I compute the distances between queries using the same technique
// (weights) as the distances between documents but in query space. I calculate the distances
// between documents and between documents and queries using the main RM approach, so they
// are computed in answer space. That way the distances are actually from two different spaces
// and should not be comparable.
//
// However, if you think about it, is sort of makes sense: I visualize distances among
// documents, then I visualize how queries relate to the documents and visualize how queries relate
// to each other.
//
// Also, were I to compute query-query distances as distances between query objects
// in document space (see distances_in_answer_space), the distances between queries
// are lowered (artificially?) because of the links to documents.

// if I'm to visualize the distances -- the query-query distances will

namespace jerome { namespace ir { namespace rm {

	template <class Ranker>
	struct DistanceComputationInMixedSpace {
		
		static bool compute(Ranker& inRanker, DistanceComputationDelegate& ioDelegate) {
			
			double progress = 0;
			if (!ioDelegate.noteProgress(progress)) return false;
			
			const std::size_t	ndocs	= inRanker.countOfDocuments();
			const std::size_t	nqrys	= inRanker.countOfQueries();
			
			double	totalProgress
			= 1 // computeAffinity for documents
			+ 1 // computeQueryModelsForAllQueries
			+ 1 // computeAffinity
			+ 1 // compute distances for docs
			+ 1 // compute distances for queries
			+ nqrys;
			
			double	deltaProgress	= 1/totalProgress;
			
			ioDelegate.setObjectCount(ndocs+nqrys);
			
			const WeightMatrix&	A		= inRanker.document().weightings().computeAffinity(inRanker.documentWeightingContext());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			const WeightMatrix&	B		= inRanker.queryModel(inRanker.queryWeightingContext().index());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			const WeightMatrix&	C		= inRanker.document().weightings().computeAffinity(inRanker.queryWeightingContext());
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			SymmetricWeightMatrix			D(ndocs+nqrys, ndocs+nqrys);
			SymmetricWeightMatrixRange		DD(D, jerome::Range(0,ndocs), jerome::Range(0,ndocs));
			SymmetricWeightMatrixRange		QQ(D, jerome::Range(ndocs,ndocs+nqrys), jerome::Range(ndocs,ndocs+nqrys));
			SymmetricWeightMatrixRange		DQ(D, jerome::Range(0,ndocs), jerome::Range(ndocs,ndocs+nqrys));
			WeightVector			diagA = diag(A);
			WeightVector			diagC = diag(C);
			WeightScalarVector		Ed(ndocs, 1);
			WeightScalarVector		Eq(nqrys, 1);
			
			DistanceComputationDelegate& R = ioDelegate;
			
			DD = outer_prod(diagA, Ed) - A + outer_prod(Ed, diagA) - trans(A);
			
//			log::info() << "DD ==========================================================";
//      log::info() << DD;
			
			
			//					for(std::size_t i = 0, n = ndocs; i < n; ++i) {
			//						for(std::size_t j = 0; j <= i; ++j) {
			//							R(i,j) = sqrt(fabs(A(i,i) - A(i,j) + A(j,j) - A(j,i)));
			//						}
			//					}
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			QQ = outer_prod(diagC, Eq) - C + outer_prod(Eq, diagC) - trans(C);
//			log::info() << "QQ ==========================================================";
//      log::info() << QQ;
			
			//					for(std::size_t i = 0, n = nqrys; i < n; ++i) {
			//						for(std::size_t j = 0; j <= i; ++j) {
			//							R(i+ndocs,j+ndocs) = sqrt(fabs(C(i,i) - C(i,j) + C(j,j) - C(j,i)));
			//						}
			//					}
			
			if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			
			DQ = inRanker.prod(A, B);
//			log::info() << "DQ ==========================================================";
//      log::info() << DQ;
			
//			log::info() << "Ae ==========================================================";
//      log::info() << Ae;
//
//			log::info() << "B ==========================================================";
//      log::info() << B;
			
			//					for(std::size_t i = 0, n = nqrys; i < n; ++i) {
			//						WeightMatrixConstColumn	column(B, i);
			//						for(std::size_t j = 0; j < ndocs; ++j) {
			//							WeightMatrixConstRow	a(A, j);
			//							R(i+ndocs,j) = sqrt(fabs(2*mDocumentContext.dotOperator()(column, a)));
			//						}
			//
			//						if (!ioDelegate.noteProgress(progress += deltaProgress)) return false;
			//					}
			
			for(std::size_t i = 0, n = D.size1(); i < n; ++i) {
				for(std::size_t j = 0; j <= i; ++j) {
					R(i,j) = sqrt(fabs(D(i,j)));
					//							R(i,j) = sqrt(fabs(R(i,j))); // the distance is a sqrt(KL(Q|D) + KL(D|Q)) I'm taking abs() just in case... :)
				}
			}
			
			
			ioDelegate.noteProgress(1);
			return true;
		}
		
	};

}}}

#endif // defined __jerome_ir_rm_distances_in_mixed_space_hpp__
