#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <cfloat>
#include <climits>
#include <unordered_map>
#include <typeinfo>
#include <mpi.h>
#include "InputKey.h"

namespace po = boost::program_options;

#define CHECK_AND_FIX 0
#define CHECK_AND_TERMINATE 1
#define REQUIRED true
#if (defined(_WIN32) || defined(_WIN64))
  #undef OPTIONAL
#endif

#define OPTIONAL false

#define CONTROL_OPTIONS 0
#define KS_SOLVER_OPTIONS 1
#define OCCUPATION_OPTIONS 2
#define MIXING_OPTIONS 3
#define MD_OPTIONS 4
#define DIAG_OPTIONS 5
#define PERF_OPTIONS 6
#define LDAU_OPTIONS 7 
#define MISC_OPTIONS 8 

class RmgInputFile {
    public:
        RmgInputFile(char *inputfile, std::unordered_map<std::string, InputKey *>& Map, MPI_Comm comm);
        ~RmgInputFile(void);

        template <typename T>
        void RegisterInputKey(std::string KeyName, T *Readval, T Minval, T Maxval, T Defval, bool Fix, bool Required, const char *helpmsg, const char *errmsg);
        void RegisterInputKey(std::string KeyName, std::string *Readstr, const char *defstr, bool Fix, bool Required, const char *helpmsg, const char *errmsg);
        void RegisterInputKey(std::string KeyName, std::string *Readstr, int *Readval, const char *defstr, bool Fix, bool Required, 
                              const std::unordered_map<std::string, int>& Allowed, const char *helpmsg, const char *errmsg);
        void RegisterInputKey(std::string KeyName, RmgInput::ReadVector<int> *V, RmgInput::ReadVector<int> *Defintvec, int count, bool Required, 
                              const char *helpmsg, const char *errmsg);
        void RegisterInputKey(std::string KeyName, RmgInput::ReadVector<double> *V, RmgInput::ReadVector<double> *Defdblvec, int count, bool Required, 
                              const char *helpmsg, const char *errmsg);
        void RegisterInputKey(std::string KeyName, bool *ReadVal, bool Defval, const char *helpmsg);

        template <typename T>
        void RegisterInputKey(std::string KeyName, T *Readval, T Minval, T Maxval, T Defval, bool Fix, bool Required, const char *helpmsg, const char *errmsg, int group);
        void RegisterInputKey(std::string KeyName, std::string *Readstr, const char *defstr, bool Fix, bool Required, const char *helpmsg, const char *errmsg, int group);
        void RegisterInputKey(std::string KeyName, std::string *Readstr, int *Readval, const char *defstr, bool Fix, bool Required, 
                              const std::unordered_map<std::string, int>& Allowed, const char *helpmsg, const char *errmsg, int group);
        void RegisterInputKey(std::string KeyName, RmgInput::ReadVector<int> *V, RmgInput::ReadVector<int> *Defintvec, int count, bool Required, 
                              const char *helpmsg, const char *errmsg, int group);
        void RegisterInputKey(std::string KeyName, RmgInput::ReadVector<double> *V, RmgInput::ReadVector<double> *Defdblvec, int count, bool Required, 
                              const char *helpmsg, const char *errmsg, int group);
        void RegisterInputKey(std::string KeyName, bool *ReadVal, bool Defval, const char *helpmsg, int group);


        void LoadInputKeys(void);

    private:
        void PreprocessInputFile(char *cfile, MPI_Comm comm);
        po::options_description control;
        std::unordered_map<std::string, InputKey *>& InputMap;
        std::unordered_map<std::string, std::string> InputPairs;
        po::variables_map vm;

};


