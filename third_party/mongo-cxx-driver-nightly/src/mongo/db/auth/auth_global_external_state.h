/*
*    Copyright (C) 2012 10gen Inc.
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

#include <string>

#include "mongo/base/disallow_copying.h"
#include "mongo/base/status.h"
#include "mongo/db/auth/user_name.h"
#include "mongo/db/jsobj.h"

namespace mongo {

    /**
     * Public interface for a class that encapsulates all the information related to system
     * state not stored in AuthorizationManager.  This is primarily to make AuthorizationManager
     * easier to test as well as to allow different implementations for mongos and mongod.
     */
    class AuthGlobalExternalState {
        MONGO_DISALLOW_COPYING(AuthGlobalExternalState);

    public:

        virtual ~AuthGlobalExternalState();

        // Gets the privilege information document for "userName" on "dbname".
        //
        // On success, returns Status::OK() and stores a shared-ownership copy of the document into
        // "result".
        Status getPrivilegeDocument(const std::string& dbname,
                                    const UserName& userName,
                                    BSONObj* result);

    protected:
        AuthGlobalExternalState(); // This class should never be instantiated directly.

        // Queries the userNamespace with the given query and returns the privilegeDocument found
        // in *result.  Returns true if it finds a document matching the query, or false if not.
        virtual bool _findUser(const std::string& usersNamespace,
                               const BSONObj& query,
                               BSONObj* result) const = 0;

        // Returns true if there exists at least one privilege document in the given database.
        bool _hasPrivilegeDocument(const std::string& dbname) const;
    };

} // namespace mongo
