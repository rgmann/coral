/**
 *    Copyright (C) 2013 10gen Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <boost/scoped_ptr.hpp>
#include <string>

#include "mongo/base/disallow_copying.h"
#include "mongo/bson/mutable/element.h"
#include "mongo/db/field_ref.h"
#include "mongo/db/jsobj.h"
#include "mongo/db/ops/modifier_interface.h"
#include "mongo/db/ops/modifier_push_sorter.h"

namespace mongo {

    class ModifierPush : public ModifierInterface {
        MONGO_DISALLOW_COPYING(ModifierPush);

    public:

        ModifierPush();

        //
        // Modifier interface implementation
        //

        virtual ~ModifierPush();

        /**
         * A 'modExpr' here is a BSONElement {<fieldname>: <each clause>, <slice clause>, <sort
         * clause>} coming from a $push mod such as {$set: {x: $each[{a:1}], $slice:3,
         * $sort{b:1}}}. init() extracts and validates the field name and the clauses. It
         * returns OK if successful or a status describing the error.
         *
         * There are currently a few restrictions concerning the clauses (but all can be
         * lifted):
         *   + $slice can be negative only (ie, slicing from the recent end)
         *   + $sort requires $slice to be present
         *   + $sort can only sort objects (as opposed to basic types), so it only takes
         *     object as patterns
         *   + Because of the previous, $sort requires that the array being pushed to be made
         *     of objects
         */
        virtual Status init(const BSONElement& modExpr);

        /**
         * Locates the array to be pushed into in the 'root', if it exists, and fills in
         * execInfo accordingly. Returns true if $push would succeed in 'root', otherwise
         * return a status describing the error.
         *
         * Note that a $push is never in-place. The cost of checking if it is a no-op makes it
         * so that we don't do such check either. As such, execInfo is always filled with
         * 'false' for those two options.
         */
        virtual Status prepare(mutablebson::Element root,
                               const StringData& matchedField,
                               ExecInfo* execInfo);

        /**
         * Pushes the array into the prepared position and "sort/slice"s the resulting array
         * according to that call's instructions.
         */
        virtual Status apply() const;

        /**
         * $push currently logs the entire resulting array as a $set.
         *
         * TODO Log a positional $set in the array, whenever possible.
         */
        virtual Status log(mutablebson::Element logRoot) const;

    private:

        // Access to each component of fieldName that's the target of this mod.
        FieldRef _fieldRef;

        // 0 or index for $-positional in _fieldRef.
        size_t _posDollar;

        // Clauses for the $push that are filled when the $each variation of the command is used.
        bool _eachMode;
        BSONElement _eachElem;
        bool _slicePresent;
        int64_t _slice;
        bool _sortPresent;
        PatternElementCmp _sort;

        // Simple (old style) push value when the $each variation of the command is not
        // used. The _eachMode flag would be off if we're this mode.
        BSONElement _val;

        // The instance of the field in the provided doc. This state is valid after a
        // prepare() was issued and until a log() is issued. The document this mod is
        // being prepared against must be live throughout all the calls.
        struct PreparedState;
        scoped_ptr<PreparedState> _preparedState;

    };

} // namespace mongo
