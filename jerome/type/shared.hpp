//
//  shared.hpp
//
//  Created by Anton Leuski on 9/11/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_type_shared_hpp
#define __jerome_type_shared_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/string.hpp>

#pragma clang diagnostic pop

namespace jerome {
  namespace shared {
    typedef boost::interprocess::managed_mapped_file::segment_manager  segment_manager_t;
    typedef boost::interprocess::allocator<void, segment_manager_t> void_allocator;
    typedef void_allocator::rebind<char>::other char_allocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>   string;

    inline string to_string(const std::string& inString, const string::allocator_type inAllocator) {
      return string(inString.begin(), inString.end(), inAllocator);
    }

    inline string to_string(const char* inString, const string::allocator_type inAllocator) {
      return string(inString, inAllocator);
    }

    enum Access {
      read_only, write_shared, write_private
    };

    struct MappedFile {
      MappedFile(Access inAccess, const char* inPath, std::size_t inInitialSize = 1024*4)
      : mFile(inAccess == write_shared
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create, inPath, inInitialSize)
              : inAccess == read_only
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_read_only, inPath)
              : boost::interprocess::managed_mapped_file(boost::interprocess::open_copy_on_write, inPath))
      , mAllocator(mFile.get_segment_manager())
      {}

      ~MappedFile()
      {
        mFile.flush();
      }

      const shared::void_allocator& allocator() const { return mAllocator; }
      const boost::interprocess::managed_mapped_file& file() const { return mFile; }
      boost::interprocess::managed_mapped_file& file() { return mFile; }

    private:
      boost::interprocess::managed_mapped_file mFile;
      shared::void_allocator mAllocator;
    };

    template <typename T>
    struct MappedPointer {
      typedef T element_type;
      typedef element_type* pointer_type;

      const Access access;
      const std::string path;
      const std::size_t initialSize;

      MappedPointer()
      : access(Access::read_only)
      , path("")
      , initialSize(0)
      , mFile(nullptr)
      , mObject(nullptr)
      {}

      MappedPointer(Access inAccess,
                    const char* inPath,
                    std::size_t inInitialSize = 1024*4)
      : access(inAccess)
      , path(inPath)
      , initialSize(inInitialSize)
      {loadObject();}

      MappedPointer(Access inAccess,
                    const std::string& inPath,
                    std::size_t inInitialSize = 1024*4)
      : access(inAccess)
      , path(inPath)
      , initialSize(inInitialSize)
      {loadObject();}

      pointer_type get() { return mObject; }
      const pointer_type get() const { return mObject; }

      void grow(std::size_t inAdditionalBytes) {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::grow(path.c_str(),
                                                       inAdditionalBytes);
        loadObject();
      }

      void shrinkToFit() {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::shrink_to_fit(path.c_str());
        loadObject();
      }

      std::size_t storageSize() const {
        return mFile ? mFile->file().get_size() : 0;
      }

      operator bool () const { return (bool)mFile; }

    private:
      std::unique_ptr<MappedFile> mFile;
      pointer_type mObject;

      const shared::void_allocator& allocator() const { return mFile->allocator(); }
      const boost::interprocess::managed_mapped_file& file() const { return mFile->file(); }
      boost::interprocess::managed_mapped_file& file() { return mFile->file(); }

      void loadObject() {
        mFile = std::unique_ptr<MappedFile>(new MappedFile(access,
                                                           path.c_str(),
                                                           initialSize));
        auto objectName = "Root";
        mObject = access == Access::read_only
        ? mFile->file().find<element_type>(objectName).first
        : mFile->file()
        .find_or_construct<element_type>(objectName)(typename element_type::ctor_args_list(),
                                                     mFile->allocator());
      }
    };
  }

}

namespace std {
  inline std::string to_string(const jerome::shared::string& inString) {
    return std::string(inString.begin(), inString.end());
  }
}

#endif // defined __jerome_type_shared_hpp
