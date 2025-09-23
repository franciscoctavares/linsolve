#ifndef FLAG_H
#define FLAG_H

#include <string>
#include <optional>
#include <vector>

class Flag {
    public:
       Flag(const std::optional<std::string> shortF = std::nullopt, const std::optional<std::string> longF = std::nullopt);

       const std::optional<std::string>& getShortForm() const { return shortForm; }
       const std::optional<std::string>& getLongForm() const { return longForm; }

       /**
        * @brief Returns true if only if flag(`shortForm` or `longForm`) is found in `args`
        * 
        * @throw drgdrg
        */
       bool parseFlag(const std::vector<std::string>& args) const;

    private:
        const std::optional<std::string> shortForm;  // example: -h
        const std::optional<std::string> longForm;   // example: --help    
};

#endif