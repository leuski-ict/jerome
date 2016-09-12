//
//  Train.hpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef Train_hpp
#define Train_hpp

#include "Command.hpp"

class Train : public Command {
public:
  Train();
private:
  std::string description() const override;
  void run(const po::variables_map& vm) override;
};


#endif /* Train_hpp */
