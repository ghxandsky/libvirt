/*
 * qemu_command.h: QEMU command generation
 *
 * Copyright (C) 2006-2013 Red Hat, Inc.
 * Copyright (C) 2006 Daniel P. Berrange
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: Daniel P. Berrange <berrange@redhat.com>
 */

#ifndef __QEMU_COMMAND_H__
# define __QEMU_COMMAND_H__

# include "domain_conf.h"
# include "vircommand.h"
# include "capabilities.h"
# include "qemu_conf.h"
# include "qemu_domain.h"
# include "qemu_capabilities.h"

/* Config type for XML import/export conversions */
# define QEMU_CONFIG_FORMAT_ARGV "qemu-argv"

# define QEMU_DRIVE_HOST_PREFIX "drive-"
# define QEMU_VIRTIO_SERIAL_PREFIX "virtio-serial"
# define QEMU_FSDEV_HOST_PREFIX "fsdev-"

/* These are only defaults, they can be changed now in qemu.conf and
 * explicitely specified port is checked against these two (makes
 * sense to limit the values).
 *
 * This limitation is mentioned in qemu.conf, so bear in mind that the
 * configuration file should reflect any changes made to these values.
 */
# define QEMU_REMOTE_PORT_MIN  5900
# define QEMU_REMOTE_PORT_MAX  65535


virCommandPtr qemuBuildCommandLine(virConnectPtr conn,
                                   virQEMUDriverPtr driver,
                                   virDomainDefPtr def,
                                   virDomainChrSourceDefPtr monitor_chr,
                                   bool monitor_json,
                                   virQEMUCapsPtr qemuCaps,
                                   const char *migrateFrom,
                                   int migrateFd,
                                   virDomainSnapshotObjPtr current_snapshot,
                                   enum virNetDevVPortProfileOp vmop)
    ATTRIBUTE_NONNULL(1);

/* Generate string for arch-specific '-device' parameter */
char *
qemuBuildChrDeviceStr (virDomainChrDefPtr serial,
                       virQEMUCapsPtr qemuCaps,
                       virArch arch,
                       char *machine);

/* With vlan == -1, use netdev syntax, else old hostnet */
char * qemuBuildHostNetStr(virDomainNetDefPtr net,
                           virQEMUDriverPtr driver,
                           char type_sep,
                           int vlan,
                           const char *tapfd,
                           const char *vhostfd);

/* Legacy, pre device support */
char * qemuBuildNicStr(virDomainNetDefPtr net,
                       const char *prefix,
                       int vlan);

/* Current, best practice */
char * qemuBuildNicDevStr(virDomainNetDefPtr net,
                          int vlan,
                          int bootindex,
                          virQEMUCapsPtr qemuCaps);

char *qemuDeviceDriveHostAlias(virDomainDiskDefPtr disk,
                               virQEMUCapsPtr qemuCaps);

/* Both legacy & current support */
char *qemuBuildDriveStr(virConnectPtr conn,
                        virDomainDiskDefPtr disk,
                        bool bootable,
                        virQEMUCapsPtr qemuCaps);
char *qemuBuildFSStr(virDomainFSDefPtr fs,
                     virQEMUCapsPtr qemuCaps);

/* Current, best practice */
char * qemuBuildDriveDevStr(virDomainDefPtr def,
                            virDomainDiskDefPtr disk,
                            int bootindex,
                            virQEMUCapsPtr qemuCaps);
char * qemuBuildFSDevStr(virDomainFSDefPtr fs,
                         virQEMUCapsPtr qemuCaps);
/* Current, best practice */
char * qemuBuildControllerDevStr(virDomainDefPtr domainDef,
                                 virDomainControllerDefPtr def,
                                 virQEMUCapsPtr qemuCaps,
                                 int *nusbcontroller);

char * qemuBuildWatchdogDevStr(virDomainWatchdogDefPtr dev,
                               virQEMUCapsPtr qemuCaps);

char * qemuBuildMemballoonDevStr(virDomainMemballoonDefPtr dev,
                                 virQEMUCapsPtr qemuCaps);

char * qemuBuildUSBInputDevStr(virDomainInputDefPtr dev,
                               virQEMUCapsPtr qemuCaps);

char * qemuBuildSoundDevStr(virDomainSoundDefPtr sound,
                            virQEMUCapsPtr qemuCaps);

/* Legacy, pre device support */
char * qemuBuildPCIHostdevPCIDevStr(virDomainHostdevDefPtr dev);
/* Current, best practice */
char * qemuBuildPCIHostdevDevStr(virDomainHostdevDefPtr dev,
                                 const char *configfd,
                                 virQEMUCapsPtr qemuCaps);

int qemuOpenPCIConfig(virDomainHostdevDefPtr dev);

/* Legacy, pre device support */
char * qemuBuildUSBHostdevUsbDevStr(virDomainHostdevDefPtr dev);
/* Current, best practice */
char * qemuBuildUSBHostdevDevStr(virDomainHostdevDefPtr dev,
                                 virQEMUCapsPtr qemuCaps);

char * qemuBuildHubDevStr(virDomainHubDefPtr dev, virQEMUCapsPtr qemuCaps);
char * qemuBuildRedirdevDevStr(virDomainDefPtr def,
                               virDomainRedirdevDefPtr dev,
                               virQEMUCapsPtr qemuCaps);

int qemuNetworkIfaceConnect(virDomainDefPtr def,
                            virConnectPtr conn,
                            virQEMUDriverPtr driver,
                            virDomainNetDefPtr net,
                            virQEMUCapsPtr qemuCaps)
    ATTRIBUTE_NONNULL(2);

int qemuPhysIfaceConnect(virDomainDefPtr def,
                         virQEMUDriverPtr driver,
                         virDomainNetDefPtr net,
                         virQEMUCapsPtr qemuCaps,
                         enum virNetDevVPortProfileOp vmop);

int qemuOpenVhostNet(virDomainDefPtr def,
                     virDomainNetDefPtr net,
                     virQEMUCapsPtr qemuCaps,
                     int *vhostfd);

int qemuNetworkPrepareDevices(virDomainDefPtr def);

/*
 * NB: def->name can be NULL upon return and the caller
 * *must* decide how to fill in a name in this case
 */
virDomainDefPtr qemuParseCommandLine(virCapsPtr qemuCaps,
                                     virDomainXMLOptionPtr xmlopt,
                                     const char **progenv,
                                     const char **progargv,
                                     char **pidfile,
                                     virDomainChrSourceDefPtr *monConfig,
                                     bool *monJSON);
virDomainDefPtr qemuParseCommandLineString(virCapsPtr qemuCaps,
                                           virDomainXMLOptionPtr xmlopt,
                                           const char *args,
                                           char **pidfile,
                                           virDomainChrSourceDefPtr *monConfig,
                                           bool *monJSON);
virDomainDefPtr qemuParseCommandLinePid(virCapsPtr qemuCaps,
                                        virDomainXMLOptionPtr xmlopt,
                                        pid_t pid,
                                        char **pidfile,
                                        virDomainChrSourceDefPtr *monConfig,
                                        bool *monJSON);

int qemuDomainAssignAddresses(virDomainDefPtr def,
                              virQEMUCapsPtr qemuCaps,
                              virDomainObjPtr obj)
    ATTRIBUTE_NONNULL(1) ATTRIBUTE_NONNULL(2);
int qemuDomainAssignSpaprVIOAddresses(virDomainDefPtr def,
                                      virQEMUCapsPtr qemuCaps);

int qemuDomainAssignPCIAddresses(virDomainDefPtr def,
                                 virQEMUCapsPtr qemuCaps,
                                 virDomainObjPtr obj);
qemuDomainPCIAddressSetPtr qemuDomainPCIAddressSetCreate(virDomainDefPtr def,
                                                         unsigned int nbuses,
                                                         bool dryRun);
int qemuDomainPCIAddressReserveSlot(qemuDomainPCIAddressSetPtr addrs,
                                    virDevicePCIAddressPtr addr);
int qemuDomainPCIAddressReserveAddr(qemuDomainPCIAddressSetPtr addrs,
                                    virDevicePCIAddressPtr addr);
int qemuDomainPCIAddressSetNextAddr(qemuDomainPCIAddressSetPtr addrs,
                                    virDomainDeviceInfoPtr dev);
int qemuDomainPCIAddressEnsureAddr(qemuDomainPCIAddressSetPtr addrs,
                                   virDomainDeviceInfoPtr dev);
int qemuDomainPCIAddressReleaseAddr(qemuDomainPCIAddressSetPtr addrs,
                                    virDevicePCIAddressPtr addr);
int qemuDomainPCIAddressReleaseSlot(qemuDomainPCIAddressSetPtr addrs,
                                    virDevicePCIAddressPtr addr);

void qemuDomainPCIAddressSetFree(qemuDomainPCIAddressSetPtr addrs);
int  qemuAssignDevicePCISlots(virDomainDefPtr def,
                              virQEMUCapsPtr qemuCaps,
                              qemuDomainPCIAddressSetPtr addrs);

int qemuDomainCCWAddressReleaseAddr(qemuDomainCCWAddressSetPtr addrs,
                                    virDomainDeviceInfoPtr dev);
int qemuDomainCCWAddressAssign(virDomainDeviceInfoPtr dev, qemuDomainCCWAddressSetPtr addrs,
                               bool autoassign);
void qemuDomainCCWAddressSetFree(qemuDomainCCWAddressSetPtr addrs);

int qemuAssignDeviceAliases(virDomainDefPtr def, virQEMUCapsPtr qemuCaps);
int qemuDomainNetVLAN(virDomainNetDefPtr def);
int qemuAssignDeviceNetAlias(virDomainDefPtr def, virDomainNetDefPtr net, int idx);
int qemuAssignDeviceDiskAlias(virDomainDefPtr vmdef,
                              virDomainDiskDefPtr def,
                              virQEMUCapsPtr qemuCaps);
int qemuAssignDeviceHostdevAlias(virDomainDefPtr def, virDomainHostdevDefPtr hostdev, int idx);
int qemuAssignDeviceControllerAlias(virDomainControllerDefPtr controller);
int qemuAssignDeviceRedirdevAlias(virDomainDefPtr def, virDomainRedirdevDefPtr redirdev, int idx);

int
qemuParseKeywords(const char *str,
                  char ***retkeywords,
                  char ***retvalues,
                  int allowEmptyValue);

#endif /* __QEMU_COMMAND_H__*/
