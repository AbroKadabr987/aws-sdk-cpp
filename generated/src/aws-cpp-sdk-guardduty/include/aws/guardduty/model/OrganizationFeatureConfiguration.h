﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <aws/guardduty/GuardDuty_EXPORTS.h>
#include <aws/guardduty/model/OrgFeature.h>
#include <aws/guardduty/model/OrgFeatureStatus.h>
#include <aws/core/utils/memory/stl/AWSVector.h>
#include <aws/guardduty/model/OrganizationAdditionalConfiguration.h>
#include <utility>

namespace Aws
{
namespace Utils
{
namespace Json
{
  class JsonValue;
  class JsonView;
} // namespace Json
} // namespace Utils
namespace GuardDuty
{
namespace Model
{

  /**
   * <p>A list of features which will be configured for the
   * organization.</p><p><h3>See Also:</h3>   <a
   * href="http://docs.aws.amazon.com/goto/WebAPI/guardduty-2017-11-28/OrganizationFeatureConfiguration">AWS
   * API Reference</a></p>
   */
  class OrganizationFeatureConfiguration
  {
  public:
    AWS_GUARDDUTY_API OrganizationFeatureConfiguration();
    AWS_GUARDDUTY_API OrganizationFeatureConfiguration(Aws::Utils::Json::JsonView jsonValue);
    AWS_GUARDDUTY_API OrganizationFeatureConfiguration& operator=(Aws::Utils::Json::JsonView jsonValue);
    AWS_GUARDDUTY_API Aws::Utils::Json::JsonValue Jsonize() const;


    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline const OrgFeature& GetName() const{ return m_name; }

    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline bool NameHasBeenSet() const { return m_nameHasBeenSet; }

    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline void SetName(const OrgFeature& value) { m_nameHasBeenSet = true; m_name = value; }

    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline void SetName(OrgFeature&& value) { m_nameHasBeenSet = true; m_name = std::move(value); }

    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithName(const OrgFeature& value) { SetName(value); return *this;}

    /**
     * <p>The name of the feature that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithName(OrgFeature&& value) { SetName(std::move(value)); return *this;}


    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline const OrgFeatureStatus& GetAutoEnable() const{ return m_autoEnable; }

    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline bool AutoEnableHasBeenSet() const { return m_autoEnableHasBeenSet; }

    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline void SetAutoEnable(const OrgFeatureStatus& value) { m_autoEnableHasBeenSet = true; m_autoEnable = value; }

    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline void SetAutoEnable(OrgFeatureStatus&& value) { m_autoEnableHasBeenSet = true; m_autoEnable = std::move(value); }

    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithAutoEnable(const OrgFeatureStatus& value) { SetAutoEnable(value); return *this;}

    /**
     * <p>The status of the feature that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithAutoEnable(OrgFeatureStatus&& value) { SetAutoEnable(std::move(value)); return *this;}


    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline const Aws::Vector<OrganizationAdditionalConfiguration>& GetAdditionalConfiguration() const{ return m_additionalConfiguration; }

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline bool AdditionalConfigurationHasBeenSet() const { return m_additionalConfigurationHasBeenSet; }

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline void SetAdditionalConfiguration(const Aws::Vector<OrganizationAdditionalConfiguration>& value) { m_additionalConfigurationHasBeenSet = true; m_additionalConfiguration = value; }

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline void SetAdditionalConfiguration(Aws::Vector<OrganizationAdditionalConfiguration>&& value) { m_additionalConfigurationHasBeenSet = true; m_additionalConfiguration = std::move(value); }

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithAdditionalConfiguration(const Aws::Vector<OrganizationAdditionalConfiguration>& value) { SetAdditionalConfiguration(value); return *this;}

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& WithAdditionalConfiguration(Aws::Vector<OrganizationAdditionalConfiguration>&& value) { SetAdditionalConfiguration(std::move(value)); return *this;}

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& AddAdditionalConfiguration(const OrganizationAdditionalConfiguration& value) { m_additionalConfigurationHasBeenSet = true; m_additionalConfiguration.push_back(value); return *this; }

    /**
     * <p>The additional information that will be configured for the organization.</p>
     */
    inline OrganizationFeatureConfiguration& AddAdditionalConfiguration(OrganizationAdditionalConfiguration&& value) { m_additionalConfigurationHasBeenSet = true; m_additionalConfiguration.push_back(std::move(value)); return *this; }

  private:

    OrgFeature m_name;
    bool m_nameHasBeenSet = false;

    OrgFeatureStatus m_autoEnable;
    bool m_autoEnableHasBeenSet = false;

    Aws::Vector<OrganizationAdditionalConfiguration> m_additionalConfiguration;
    bool m_additionalConfigurationHasBeenSet = false;
  };

} // namespace Model
} // namespace GuardDuty
} // namespace Aws
