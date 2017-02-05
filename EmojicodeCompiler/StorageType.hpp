//
//  StorageType.hpp
//  Emojicode
//
//  Created by Theo Weidmann on 05/01/2017.
//  Copyright © 2017 Theo Weidmann. All rights reserved.
//

#ifndef StorageType_hpp
#define StorageType_hpp

enum class StorageType {
    Simple,
    SimpleOptional,
    Box,
    /// Value should be unboxed if possible
    SimpleIfPossible
};

#endif /* StorageType_hpp */
