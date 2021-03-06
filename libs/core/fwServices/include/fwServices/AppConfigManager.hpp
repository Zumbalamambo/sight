/************************************************************************
 *
 * Copyright (C) 2015-2020 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwServices/config.hpp"
#include "fwServices/helper/Config.hpp"
#include "fwServices/helper/ProxyConnections.hpp"
#include "fwServices/IAppConfigManager.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/registry/AppConfig.hpp"

#include <fwCom/HasSlots.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fwData
{
class Composite;
}

namespace fwServices
{

/**
 * @brief This class provides an API to manage config template.
 *
 * @section Slots Slots
 * - \b addObject(fwData::Object::sptr, const std::string&): adds objects to the configuration.
 * - \b removeObject(fwData::Object::sptr, const std::string&): removes objects from the configuration.
 */
class FWSERVICES_CLASS_API AppConfigManager :  public ::fwServices::IAppConfigManager,
                                               public ::fwCom::HasSlots
{
public:

    fwCoreClassMacro(AppConfigManager, ::fwServices::IAppConfigManager, std::make_shared< AppConfigManager >)

    fwCoreAllowSharedFromThis()

    /// Creates slots.
    FWSERVICES_API AppConfigManager();

    /// Does nothing.
    FWSERVICES_API virtual ~AppConfigManager();

    /**
     * @brief Sets configuration.
     * @param _configId The identifier of the requested config.
     * @param _replaceFields The associations between the value and the pattern to replace in the config.
     */
    FWSERVICES_API virtual void setConfig(const std::string& _configId,
                                          const FieldAdaptorType& _replaceFields = FieldAdaptorType()) override;

    /**
     * @brief Sets configuration.
     * @param _configId The identifier of the requested config.
     * @param _replaceFields Composite of association between the value and the pattern to replace in the config.
     */
    FWSERVICES_API virtual void setConfig(const std::string& _configId,
                                          const ::fwData::Composite::csptr& _replaceFields) override;

    /**
     * @brief Get the configuraton root.
     * @return The configuration root.
     */
    FWSERVICES_API virtual ::fwData::Object::sptr getConfigRoot() const override;

    /// Calls methods : create, start then update.
    FWSERVICES_API virtual void launch() override;

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    FWSERVICES_API virtual void stopAndDestroy() override;

    /// Creates objects and services from config.
    FWSERVICES_API virtual void create() override;

    /// Starts services specified in config.
    FWSERVICES_API virtual void start() override;

    /// Updates services specified in config.
    FWSERVICES_API virtual void update() override;

    /// Stops services specified in config.
    FWSERVICES_API virtual void stop() override;

    /// Destroys services specified in config.
    FWSERVICES_API virtual void destroy() override;

    /**
     * @brief Starts the module associated to the config
     * @note  Does nothing if the module is already started or if the config id is not specified (ie. if config is set
     *        with setConfig(::fwRuntime::ConfigurationElement::csptr cfgElem) ).
     */
    FWSERVICES_API virtual void startBundle();

    /**
     * @brief Sets if we are testing the class.
     * @param _isUnitTest Use true to set it as a test.
     */
    FWSERVICES_API void setIsUnitTest(bool _isUnitTest);

    /**
     * @brief Adds an existing deferred object to the deferred objects map.
     *
     * @pre The manager musn't be started.
     *
     * When a configuration is launched, deferred objects may already exist.
     * This loop allow to notify the app config manager that this data exist and can be used by services.
     * Whitout that, the data is considered as null.
     *
     * @param _obj The object to add.
     * @param _uid The uid of this object.
     */
    FWSERVICES_API void addExistingDeferredObject(const ::fwData::Object::sptr& _obj, const std::string& _uid);

private:

    typedef ::std::pair< std::string, bool > ConfigAttribute;
    typedef ::fwServices::helper::ProxyConnections ProxyConnections;
    typedef ::fwServices::IService::Config ServiceConfig;

    ::fwData::Object::sptr findObject(const std::string& uid, const std::string& errMsgTail) const;

    ::fwData::Object::sptr getNewObject(ConfigAttribute type, const std::string& uid) const;

    ::fwData::Object::sptr getNewObject(ConfigAttribute type,
                                        ConfigAttribute uid = ConfigAttribute("", false)) const;

    ::fwData::Object::sptr getObject(ConfigAttribute type, const std::string& uid) const;

    ::fwServices::IService::sptr getNewService(const std::string& uid, const std::string& implType) const;

    /// Stops all started services.
    void stopStartedServices();

    /// Destroyes all created services
    void destroyCreatedServices();

    void processStartItems();

    void processUpdateItems();

    /// Parses objects section and create objects.
    void createObjects(::fwRuntime::ConfigurationElement::csptr cfgElem);

    /// Parses services and create all the services that can be instantiated.
    void createServices(::fwRuntime::ConfigurationElement::csptr cfgElem);

    /// Creates a single service from its configuration.
    ::fwServices::IService::sptr createService(const ServiceConfig& srvConfig);

    /// Parses connection sections and creates them.
    void createConnections();

    /// Stops and destroys services specified in config, then resets the configRoot sptr.
    std::string msgHead() const;

    /**
     * @brief Adds objects to the configuration.
     * @param _obj The object to add.
     * @param _id The id of the object.
     */
    void addObjects(::fwData::Object::sptr _obj, const std::string& _id);

    /**
     * @brief Removes objects from the configuration.
     * @param _obj The object to remove.
     * @param _id The id of the remove.
     */
    void removeObjects(::fwData::Object::sptr _obj, const std::string& _id);

    void connectProxy(const std::string& _channel, const ProxyConnections& _connectCfg);

    void destroyProxy(const std::string& _channel, const ProxyConnections& _proxyCfg, const std::string& _key = "",
                      fwData::Object::csptr _hintObj = nullptr);

    void destroyProxies();

    /// Gets a list of UIDs or WIDs, get a friendly printable message.
    static std::string getUIDListAsString(const std::vector<std::string>& uidList);

    typedef std::pair< ::fwData::Object::sptr, ::fwServices::IXMLParser::sptr> CreatedObjectType;

    /// Map containing the object and its XML parser.
    std::unordered_map<std::string, CreatedObjectType> m_createdObjects;

    struct DeferredObjectType
    {
        std::vector< ServiceConfig > m_servicesCfg;
        std::unordered_map< std::string, ProxyConnections > m_proxyCnt;
        /// Copy of the object pointer necessary to access signals/slots when destroying proxy.
        ::fwData::Object::sptr m_object;
    };

    /// Map indexed by the object uid, containing all the service configurations that depend on this object.
    std::unordered_map<std::string, DeferredObjectType > m_deferredObjects;

    /// All the identifiers of the deferred services.
    std::unordered_set<std::string> m_deferredServices;

    /// All proxies of created objects, ordered by channel name.
    std::unordered_map<std::string, ProxyConnections> m_createdObjectsProxies;

    struct ServiceProxyType
    {
        std::unordered_map< std::string, ProxyConnections > m_proxyCnt;
    };
    std::unordered_map< std::string, ServiceProxyType > m_servicesProxies;

    /// Identifier of this configuration.
    std::string m_configId;

    typedef std::vector< ::fwServices::IService::wptr > ServiceContainer;

    /// List of services created in this configuration.
    ServiceContainer m_createdSrv;

    /// List of services started in this configuration.
    ServiceContainer m_startedSrv;

    /// Start ordered list of deferred services.
    std::vector<std::string> m_deferredStartSrv;

    /// Update ordered list of deferred services.
    std::vector<std::string> m_deferredUpdateSrv;

    /// While we need to maintain old and new services behavior, we need a dummy data for new services
    /// that don't work on any data.
    /// TODO: Remove with V1.
    ::fwData::Composite::sptr m_tmpRootObject;

    /// Counter used to generate a unique proxy name.
    unsigned int m_proxyID;

    /// Keep the connection between the OSR and `addObjects`.
    ::fwCom::Connection m_addObjectConnection;

    /// Keep the connection between the OSR and `removeObjects`.
    ::fwCom::Connection m_removeObjectConnection;

    /// Hack to know if we are doing a unit test. We skip some code in this case to be able to launch a configuration.
    bool m_isUnitTest;
};

// ------------------------------------------------------------------------

inline std::string AppConfigManager::msgHead() const
{
    return "[" + m_configId + "] ";
}

} // namespace fwServices
