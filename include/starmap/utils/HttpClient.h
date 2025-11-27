#ifndef STARMAP_HTTP_CLIENT_H
#define STARMAP_HTTP_CLIENT_H

#include <string>
#include <map>

namespace starmap {
namespace utils {

/**
 * @brief Client HTTP per query ai servizi online
 */
class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    /**
     * @brief Esegue una richiesta GET
     * @param url URL completo della richiesta
     * @param headers Headers opzionali
     * @return Risposta come stringa
     */
    std::string get(const std::string& url, 
                    const std::map<std::string, std::string>& headers = {});

    /**
     * @brief Esegue una richiesta POST
     * @param url URL completo della richiesta
     * @param data Dati da inviare
     * @param headers Headers opzionali
     * @return Risposta come stringa
     */
    std::string post(const std::string& url, 
                     const std::string& data,
                     const std::map<std::string, std::string>& headers = {});

    /**
     * @brief Imposta il timeout per le richieste
     * @param seconds Timeout in secondi
     */
    void setTimeout(long seconds);

private:
    class Impl;
    Impl* pImpl_;
};

} // namespace utils
} // namespace starmap

#endif // STARMAP_HTTP_CLIENT_H
