#ifndef RMG_InputKey_H
#define RMG_InputKey_H 1

#include <string>
#include <vector>
#include <unordered_map>



namespace RmgInput {

    template <typename VectorType>
    class ReadVector
    {
        public:
            std::vector<VectorType> vals;
    };

}


class InputKey {
    public:
        // Scalar types
        InputKey(std::string& NewKeyName, int *ReadVal, int Minval, int Maxval, int Defval, bool Fix, bool Required, const char *helpmsg, const char *errmsg, int group);
        InputKey(std::string& NewKeyName, double *ReadVal, double Minval, double Maxval, double Defval, bool Fix, bool Required, const char *helpmsg, const char *errmsg, int group);
        InputKey(std::string& NewKeyName, bool *ReadVal, bool Defval, const char *helpmsg, int group);

        // Strings
        InputKey(std::string& NewKeyName, std::string *ReadStr, const char *defstr, bool Fix, bool Required, const char *helpmsg, const char *errmsg, int group);

        // Enumerated strings
        InputKey(std::string& NewKeyName, std::string *ReadStr, int *ReadVal, const char *Defstr, bool Fix, bool Required, const std::unordered_map<std::string, int> Allowed, const char *helpmsg, const char *errmsg, int group);


        // Vectors
        InputKey(std::string& NewKeyName, RmgInput::ReadVector<int> *V , RmgInput::ReadVector<int> *Defintvec, size_t count, bool Required, const char* helpmsg, const char *errmsg, int group);
        InputKey(std::string& NewKeyName, RmgInput::ReadVector<double> *V, RmgInput::ReadVector<double> *Defdblvec, size_t count, bool Required, const char* helpmsg, const char *errmsg, int group);
        
        // Reads stored value
        std::string Print(void);

        std::string KeyName;
        std::unordered_map<std::string, int> Range;
        bool MapPresent;  // For enumerated strings
        bool allocated=false;
        size_t KeyType;
        bool Fix;
        bool Required;
        int *Readintval;
        int Minintval;
        int Maxintval;
        int Defintval;
        double *Readdoubleval;
        double Mindoubleval;
        double Maxdoubleval;
        double Defdoubleval;
        bool *Readboolval;
        bool Defboolval;
        std::string Readstr;
        std::string *Readstrorig;
        RmgInput::ReadVector<int> *Vintorig;
        RmgInput::ReadVector<double> *Vdoubleorig;
        RmgInput::ReadVector<int> Vint;
        RmgInput::ReadVector<double> Vdouble;
        RmgInput::ReadVector<int> Defintvec;
        RmgInput::ReadVector<double> Defdblvec;
        size_t count;
        const char *Defstr;
        const char* helpmsg;
        const char *errmsg;
        int grouping;
};

// Used to sort keys by group and then alphabetically
struct keycompare
{
    bool operator()(InputKey *ik_lhs, InputKey *ik_rhs)
    {
        // First sort by group
        if(ik_lhs->grouping < ik_rhs->grouping) return true;
        if(ik_lhs->grouping > ik_rhs->grouping) return false;

        // Now alphabetically by key name within group
        if(ik_lhs->KeyName.compare(ik_rhs->KeyName) < 0) return true;
        return false;
    }
};

#endif
