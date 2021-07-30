#ifndef R_EXASOL_CONNECTION_HOOK_H
#define R_EXASOL_CONNECTION_HOOK_H

namespace exa {
    namespace rconnection {
        /**
         * Stores the pointer to the Rconnection hook.
         * This allows of accessing the reader/writer from within the R connection streaming access.
         * @param con The pointer to the R_ext connection struct.
         * @param value The pointer to be stored. Can be null.
         */
        template<typename T>
        inline void storeConnectionHook(::Rconnection con, std::weak_ptr<T>* value) {
            //conn->priv allows us to store a private pointer to anything.
            //However, RExt Connections will free this memory if it is not null when cleaning up connection
            //As we want to keep control about when to delete Reader, we allocate memory for one pointer;
            //and store the pointer to the pointer of the Reader here
            auto** tmp = static_cast<std::weak_ptr<T>**>(con->priv);
            *tmp = value;
        }

        /**
         * Allocates memory for the connection hook.
         */
        template<typename T>
        inline void* allocConnectionHook() {
            return (void*)::malloc(sizeof(std::weak_ptr<T>*));
        }

        /**
         * Get's access to connection hook.
         * @param con The pointer to the R_ext connection struct.
         */
        template<typename T>
        inline std::weak_ptr<T>* getConnectionHook(const ::Rconnection con) {
            return *((std::weak_ptr<T> **) con->priv);
        }
    }
}
#endif //R_EXASOL_CONNECTION_HOOK_H
