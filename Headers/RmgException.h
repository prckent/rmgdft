/*
 *
 * Copyright (c) 2014, Emil Briggs
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
*/

#ifndef RMG_RmgException_H
#define RMG_RmgException_H 1

#include <stdexcept>
#include <sstream>
#include <string>


// Example of use
// throw fatal_error() << "Density mixing set to " ct.mix  << " but value must lie between 0.0 and 1.0";
//


// Handles errors that require program termination
class RmgFatalException : public std::exception {

private:
    mutable std::stringstream ss;
    mutable std::string What;


public:

    RmgFatalException() {};
    
    RmgFatalException( const RmgFatalException &that )
    {
        What += that.ss.str();
    }

    virtual ~RmgFatalException() throw(){};

    virtual const char *rwhat() const throw()
    {
        if ( ss.str().size() ) {
            What += ss.str();
            ss.str( "" );
        }
        return What.c_str();
    }

    template<typename T>
    RmgFatalException& operator<<( const T& t )
    {
        ss <<t;
        return *this;
     }

};


#endif


