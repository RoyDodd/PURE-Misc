/*  Copyright 2011 Geoffrey 'Phogue' Green

    This file is part of BFBC2 PRoCon.

    BFBC2 PRoCon is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BFBC2 PRoCon is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BFBC2 PRoCon.  If not, see <http://www.gnu.org/licenses/>.
 */

using System;
using System.IO;
using System.IO.Compression;
using System.Text;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Text.RegularExpressions;
using System.Net;
using System.Threading;

using PRoCon.Core;
using PRoCon.Core.Plugin;
using PRoCon.Core.Plugin.Commands;
using PRoCon.Core.Players;
using PRoCon.Core.Players.Items;
using PRoCon.Core.Battlemap;
using PRoCon.Core.Maps;

namespace PRoConEvents {

    public class Metabans : PRoConPluginAPI, IPRoConPluginInterface {
    
		private PropagationRules PropagationRule;
		private string PropagationRuleFilter;
        private MetabansDurationTypes DurationType;
        private enumBoolYesNo EnforceMetabansBans;

        private enumBoolOnOff Debug;
        
        private string Username;
        private string ApiKey;
        
        private DateTime LastSynced;
        
        private string ServerName;
        
        private int duration;

        public Metabans() {
            this.Debug = enumBoolOnOff.Off;

            this.EnforceMetabansBans = enumBoolYesNo.Yes;
			this.PropagationRule = PropagationRules.All;
			this.PropagationRuleFilter = String.Empty;
            
            this.Username = String.Empty;
            this.ApiKey = String.Empty;
            this.ServerName = String.Empty;

            this.LastSynced = DateTime.Now.AddDays(-1);
            
            //set default ban length to 4 weeks
            this.duration = 28;
            this.DurationType = MetabansDurationTypes.Days;
        }

        public string GetPluginName() {
            return "Metabans";
        }

        public string GetPluginVersion() {
            return "1.0.6.0 PURE 0.1";
        }

        public string GetPluginAuthor() {
            return "i3lack";
        }

        public string GetPluginWebsite() {
            return "metabans.com";
        }

        public string GetPluginDescription() {
            return @"
    <h2>Description</h2>
        <p>Reports and enforces bans on players.  Go to www.metabans.com to sign up for a API key.</p>
		<p>For more information about Metabans please visit the wiki at <a href=""http://wiki.metabans.com"" target=""_blank"">http://wiki.metabans.com</a></p>
        <h4><p>When using this plugin please ban by EA GUID! See -Additional Information- below.</p></h4>
    <h2>Requirements</h2>
            <h4>Plugin sandbox mode (Tools -> Options -> Plugins -> Plugin Security)</h4>
                <ul>
                    <li>URL: http://metabans.com</li>
                    <li>Port: 80</li>
                </ul>
        <p>Once you have configured your sandbox you will need to restart procon for the security policy to take effect.</p>
        <p>If you have your procon hosted you will need to nag your service provider.  These settings must be applied on the instance of procon running the plugin.</p>

    <h2>Settings</h2>
        <h3>Miscellaneous</h3>
            <blockquote><h4>Debug mode</h4>Diagnostic option to show some of the processes of the plugin</blockquote>

        <h3>Bans</h3>
            <blockquote><h4>Send Bans to Metabans</h4>Will attempt to send a new ban or unban to metabans.</blockquote>
            <blockquote><h4>Duration/Duration Type</h4>Duration length and Duration Type for your permanent bans</blockquote>
            <blockquote><h4>Enforce Metabans Bans</h4>Will enforce bans after sighting a player from metabans</blockquote>

        <h3>Authentication</h3>
            <blockquote><h4>Username</h4>The username you use to sign in to Metabans</blockquote>
            <blockquote><h4>API Key</h4>Your Metabans API key.  You can find this at <a href=""http://metabans.com/mb-profile.php#api"" target=""_blank"">http://metabans.com/mb-profile.php#api</a></blockquote>


    <h2>History</h2>
	<h4>1.0.6.0 - 03/11/13</h4>
                <ul>
					<li>Updated API Version</li>
					</ul>
	<h4>1.0.5.9 - 15/09/13</h4>
                <ul>
					<li>Now you can define DurationType for your permanent bans instead of default 4 weeks (Thanks to Mandizzy)</li>
					<li>Added initial support for Battlefield 4</li>
					<li>Updated API Version</li>
					</ul>
	<h4>1.0.5.8 - 23/10/12</h4> 
                <ul>
					<li>Added Support for Medal of Honor: Warfighter</li>
					<li>Updated API Version</li>
					</ul>			
	<h4>1.0.5.7 - 29/07/12</h4>
                <ul>
					<li>Removed Metabans message</li>
					</ul>			
		<h4>1.0.5.6 - 14/07/12</h4>
                <ul>
					<li>Corrected Version Number!</li>
					</ul>			
	
	<h4>1.0.5.4 - 14/07/12</h4>
                <ul>
					<li>Added small change thanks to Panther. Sandbox must be turned off!!</li>
					</ul>				
	<h4>1.0.5.3 - 22/06/12</h4>
                <ul>
					<li>Added Additional Information to Metabans Plugin - Details Tab (scroll to the bottom).</li>
					</ul>
	<h4>1.0.5.2 - 21/06/12</h4>
                <ul>
					<li>Created a Metabans_1.0.5.2.exe - Place the .exe in your Procon folder or Browse to your Destination Folder (Procon Install folder) and click install that's it no more placing files yourself!</li>
                    <li>Removed In-game message that metabans sends for security reasons.</li>
					</ul>
	<h4>1.0.5.0 - 28/01/12</h4>
                <ul>
					<li>Changed Ban period to (1 year) from (3 months)</li>
                    <li>Updated timeout session so admins would not reveive an instant API error.</li>
					<li>Fix for (Metabans API: Response error; Incorrect or missing api method) it wasn't really a problem, but mostly just getting rid of the error message even when the key is correct. </li>
					</ul>
	<h4>1.0.4.0 - 07/01/12</h4>
                <ul>
                    <li>Added support for Battlefield 3</li>
					</ul>
            <h4>1.0.3.0 - 07/08/11</h4>
                <ul>
                    <li>Added punkbuster kick after vanilla kick, just to reinforce vanilla (some people reporting the vanilla wasn't working)</li>
					<li>Added filter option for bans being sent to metabans.com</li>
					<li>Fix for ""Metabans API: Response error; parameter 'player_uid' of wrong type or outside value range"". Though it wasn't really a problem, but mostly just getting rid of the error message.</li>
                </ul>
            <h4>1.0.2.0 - 03/08/11</h4>
                <ul>
                    <li>Replaced two minute ban with a simple kick. We didn't think that one through, obviously ;)</li>
                </ul>
            <h4>1.0.1.0 - 21/06/11</h4>
                <ul>
                    <li>Added streaming of new bans and unbans to metabans.com (if banned by guid - see <b>Additional Information</b>)</li>
                    <li>Added option to stream bans and unbans - defaults to on</li>
                    <li>Added option to enforce bans - defaults to on</li>
                    <li>Now advertises the player, reason and your metabans banlist when a metaban is enforced</li>
                </ul>
            <h4>1.0.0.1 - 10/06/11</h4>
                <ul>
                    <li>Initial release.  Sights players and enforces metabans.</li>
                </ul>

    <h2>Additional Information</h2>
        <ul>
			<li>By Default the ban time for all uploaded bans to Metabans is 4 weeks. If you want a longer ban time it must be done via <a href=""http://metabans.com"" target=""_blank"">http://metabans.com</a> </li>
			<li>Banning and unbanning is only guaranteed to work if you do so by EA GUID.  By name it will seek out the GUID to ban with, but it won't be possible to unban players on metabans if the player was banned by their name.  The short of it is: Use EA GUID to ban with and this plugin and metabans will work pretty sweet.  Don't use the EA GUID and you will have problems with unbans and need to login to <a href=""http://metabans.com"" target=""_blank"">http://metabans.com</a> to do so properly.</li>
		</ul>";
        }

        public void OnPluginLoaded(string strHostName, string strPort, string strPRoConVersion) {
            this.RegisterEvents(
                this.GetType().Name,
                "OnPluginLoaded",
                "OnPunkbusterPlayerInfo",
                "OnPlayerLeft",
                "OnListPlayers",
                "OnPlayerJoin",
                "OnPlayerAuthenticated",
                "OnServerInfo",
                "OnBanAdded",
                "OnBanRemoved"
            );
        }

        public void OnPluginEnable() {
            this.ExecuteCommand("procon.protected.pluginconsole.write", "^bMetabans ^2Enabled!");
        }

        public void OnPluginDisable() {
            this.ExecuteCommand("procon.protected.pluginconsole.write", "^bMetabans ^1Disabled =(");
        }

        public List<CPluginVariable> GetDisplayPluginVariables() {

            List<CPluginVariable> lstReturn = new List<CPluginVariable>();

            lstReturn.Add(new CPluginVariable("Miscellaneous|Debug Mode", typeof(enumBoolOnOff), this.Debug));

            lstReturn.Add(new CPluginVariable("Bans|Enforce Metabans Bans", typeof(enumBoolYesNo), this.EnforceMetabansBans));

			lstReturn.Add(new CPluginVariable("Bans|Ban Propagation", "enum.MetabansBanPropagationRules(" + Metabans.GetDescriptionList<PropagationRules>() + ")", GetDescription<PropagationRules>(this.PropagationRule) ));
			if (this.PropagationRule == PropagationRules.Filtered) {
				lstReturn.Add(new CPluginVariable("Bans|Ban Propagation Filter", this.PropagationRuleFilter.GetType(), this.PropagationRuleFilter));
			}
            
            if (this.PropagationRule != PropagationRules.None) {
                lstReturn.Add(new CPluginVariable("Bans|Duration", this.duration.GetType(), this.duration));
                lstReturn.Add(new CPluginVariable("Bans|Duration Type", "enum.MetabansBanDurations(" + Metabans.GetDescriptionList<MetabansDurationTypes>() + ")", GetDescription<MetabansDurationTypes>(this.DurationType)));
            }
            // Commands
            lstReturn.Add(new CPluginVariable("Authentication|Username", this.Username.GetType(), this.Username));
            if (this.ApiKey.Length >= 12) { // Should be == 40 or bust..
                lstReturn.Add(new CPluginVariable("Authentication|API Key", this.ApiKey.GetType(), String.Format("{0}***{1}", ApiKey.Substring(0, 8), ApiKey.Substring(ApiKey.Length - 3, 3))));
            }
            else {
                lstReturn.Add(new CPluginVariable("Authentication|API Key", this.ApiKey.GetType(), ""));
            }
            
            return lstReturn;
        }

        // Lists all of the plugin variables.
        public List<CPluginVariable> GetPluginVariables() {
            List<CPluginVariable> lstReturn = new List<CPluginVariable>();

            lstReturn.Add(new CPluginVariable("Debug Mode", typeof(enumBoolOnOff), this.Debug));

            lstReturn.Add(new CPluginVariable("Enforce Metabans Bans", typeof(enumBoolYesNo), this.EnforceMetabansBans));

			lstReturn.Add(new CPluginVariable("Ban Propagation", "enum.MetabansBanPropagationRules(" + Metabans.GetDescriptionList<PropagationRules>() + ")", GetDescription<PropagationRules>(this.PropagationRule) ));
			lstReturn.Add(new CPluginVariable("Ban Propagation Filter", this.PropagationRuleFilter.GetType(), this.PropagationRuleFilter));

			lstReturn.Add(new CPluginVariable("Duration", this.duration.GetType(), this.duration));
            lstReturn.Add(new CPluginVariable("Duration Type", "enum.MetabansBanDurations(" + Metabans.GetDescriptionList<MetabansDurationTypes>() + ")", GetDescription<MetabansDurationTypes>(this.DurationType) ));
            
            lstReturn.Add(new CPluginVariable("Username", this.Username.GetType(), this.Username));
            lstReturn.Add(new CPluginVariable("API Key", this.ApiKey.GetType(), this.ApiKey));
            
            return lstReturn;
        }

        public void SetPluginVariable(string strVariable, string strValue) {
            int displaySeconds = 0;
            int time = 28;

            if (strVariable.CompareTo("Debug Mode") == 0 && Enum.IsDefined(typeof(enumBoolOnOff), strValue) == true) {
                this.Debug = (enumBoolOnOff)Enum.Parse(typeof(enumBoolOnOff), strValue);
            }
            else if (strVariable.CompareTo("Username") == 0) {
                this.Username = strValue;
            }
            else if (strVariable.CompareTo("Enforce Metabans Bans") == 0 && Enum.IsDefined(typeof(enumBoolYesNo), strValue) == true) {
                this.EnforceMetabansBans = (enumBoolYesNo)Enum.Parse(typeof(enumBoolYesNo), strValue);
            }
			else if (strVariable.CompareTo("Ban Propagation") == 0) {
				this.PropagationRule = Metabans.GetEnumFromDescription<PropagationRules>(strValue);
			}
			else if (strVariable.CompareTo("Ban Propagation Filter") == 0) {
				this.PropagationRuleFilter = strValue;
			}
            else if (strVariable.CompareTo("Duration") == 0 && int.TryParse(strValue, out time) == true){
                this.duration = time;
            }
            else if (strVariable.CompareTo("Duration Type") == 0) {
				this.DurationType = Metabans.GetEnumFromDescription<MetabansDurationTypes>(strValue);
			}
            // Hell, if they remove the stars that's their fault..
            // I'm sure 1 in 50 admins probably will, at least.
            else if (strVariable.CompareTo("API Key") == 0 && strVariable.IndexOf('*') == -1) {
                this.ApiKey = strValue;
            }
        }

        // Will be used when these commands are implemented - if they are implemented.
        // or @mb ban "phogue"
        // or @mb white "phogue"
        private void WriteMessage(string message, CPlayerSubset audience) {

            string strPrefix = "metabans.com >";

            List<string> wordWrappedLines = this.WordWrap(message, 100 - strPrefix.Length);

            foreach (string line in wordWrappedLines) {
                string formattedLine = String.Format("{0}{1}", strPrefix.Replace("{", "{{").Replace("}", "}}"), line);

                if (audience.Subset == CPlayerSubset.PlayerSubsetType.All) {
                    this.ExecuteCommand("procon.protected.send", "admin.say", formattedLine, "all");
                }
          else if (audience.Subset == CPlayerSubset.PlayerSubsetType.Player) {
                    this.ExecuteCommand("procon.protected.send", "admin.say", formattedLine, "player", audience.SoldierName);
                }
            }
        }

        private void SubmitAssessment(CBanInfo ban, AssessmentTypes assessmentType) {
            int length = 0;
            string playerGuid = null;

            if (this.Debug == enumBoolOnOff.On) {
                this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans (SubmitAssessment): Submitting assessment to metabans");
            }

            if (ban.IdType == "guid") {
                playerGuid = ban.Guid;
            }
            else {
                foreach(KeyValuePair<string, CPlayerInfo> player in this.FrostbitePlayerInfoList) {
                    if (ban.IdType == "name" && player.Value.SoldierName == ban.SoldierName) {
                        playerGuid = player.Value.GUID;
                        break;
                    }
                }
            }

            if (ban.BanLength.Subset == TimeoutSubset.TimeoutSubsetType.Permanent) {
                // Google says: one month = 2 628 000 seconds
                length = 2419200;

                if (Enum.IsDefined(typeof(MetabansDurationTypes), DurationType))
                {
                    switch (DurationType)
                    {
                        case MetabansDurationTypes.Days:{
                            length = this.duration * 24 * 60 * 60;
                            break;
                            }
                        case MetabansDurationTypes.Months:{
                            length = this.duration * 30 * 24 * 60 * 60;
                            break;
                            }
                        case MetabansDurationTypes.Years:{
                            length = this.duration * 365 * 24 * 60 * 60;
                            break;
                            }
                    }
                }
                    
            }
            else if (ban.BanLength.Subset == TimeoutSubset.TimeoutSubsetType.Seconds) {
                length = ban.BanLength.Seconds;
            }

            if (playerGuid != null) {
                if (this.Debug == enumBoolOnOff.On) {
                    this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans (SubmitAssessment): Submitting assessment of GUID " + playerGuid);
                }

                MetabansAPI api = new MetabansAPI(this.Username, this.ApiKey, this.Debug);
                api.ExecuteCommand += new MetabansAPI.ExecuteCommandHandler(api_ExecuteCommand);
                api.mb_assess_player_ok += new MetabansAPI.RequestSuccessHandler(api_mb_assess_player_ok);
                    
                api.mb_assess_player(
                    #include "Metabans.GameType.inc",
                    playerGuid,
                    assessmentType,
                    ban.Reason,
                    length
                ).Post();
            }
            else if (this.Debug == enumBoolOnOff.On) {
                this.ExecuteCommand("procon.protected.pluginconsole.write", "^1Metabans (SubmitAssessment): Player name not found when submitting assessment to metabans");
            }
        }

        public override void OnBanRemoved(CBanInfo ban) {
            if (this.PropagationRule != PropagationRules.None) {
                this.SubmitAssessment(ban, AssessmentTypes.none);
            }
        }

        public override void OnBanAdded(CBanInfo ban) {
			if (this.PropagationRule == PropagationRules.All) {
				this.SubmitAssessment(ban, AssessmentTypes.black);
			}
			else if (this.PropagationRule == PropagationRules.Filtered) {
				string reasonLowerCase = ban.Reason.ToLower();

				foreach (string filter in this.PropagationRuleFilter.Split(new char[] { '\r', '\n'}, StringSplitOptions.RemoveEmptyEntries)) {
					if (reasonLowerCase.IndexOf(filter.ToLower()) >= 0) {
						this.SubmitAssessment(ban, AssessmentTypes.black);

						if (this.Debug == enumBoolOnOff.On) {
							this.ExecuteCommand("procon.protected.pluginconsole.write", String.Format("^4Metabans (OnBanAdded): Filter word \"{0}\" found in \"{1}\"", filter, ban.Reason));
						}

						break;
					}
				}
			} 
        }

        public override void OnServerInfo(CServerInfo serverInfo) {
            this.ServerName = serverInfo.ServerName;
        }
        
        // Event handler for Metabans API event to execute a command.
        // Generally only needed if you are debugging something, like... MetabansAPI("", "", enumBoolOnOff.On)
        public void api_ExecuteCommand(params string[] commands) {
            this.ExecuteCommand(commands);
        }
        
        public void api_mb_assess_player_ok(Hashtable request, Hashtable data) {
            try {
                if (this.Debug == enumBoolOnOff.On) {
                    this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans (api_mb_assess_player_ok): Assessment accepted");
                }
            } catch (Exception e) {
                if (this.Debug == enumBoolOnOff.On) {
                    this.ExecuteCommand("procon.protected.pluginconsole.write", "^1Metabans (api_mb_assess_player_ok): " + e.Message);
                }
            }
        }

        public void api_mb_sight_player_ok(Hashtable request, Hashtable data) {
            try {
                if ((bool)data["is_banned"] == true && this.EnforceMetabansBans == enumBoolYesNo.Yes) {

                    string reason = "";
                
                    if ((bool)data["is_blacklisted"] == true) {
                        reason = (string)data["reason"];
                    }
                    else if (((ArrayList)data["inherited_blacklist"]).Count > 0) {
                        reason = (string)((Hashtable)((ArrayList)data["inherited_blacklist"])[0])["reason"];
                    }

                    //string message = String.Format("Enforcing ban on {0} for {1}", (string)request["player_name"], reason);
                    //this.WriteMessage(message, new CPlayerSubset(CPlayerSubset.PlayerSubsetType.All));
					

                    //this.ExecuteCommand("procon.protected.pluginconsole.write", message);

                    
                    this.ExecuteCommand("procon.protected.tasks.add", "Metabans", "10", "1", "1", "procon.protected.send", "admin.kickPlayer", (string)request["player_name"], "metabans.com: " + reason);
					this.ExecuteCommand("procon.protected.tasks.add", "Metabans", "11", "1", "1", "procon.protected.send", "punkBuster.pb_sv_command", String.Format("pb_sv_kick \"{0}\" 30 \"{1}\"", (string)request["player_name"], "metabans.com: " + reason));
                }
            } catch (Exception e) {
                if (this.Debug == enumBoolOnOff.On) {
                    this.ExecuteCommand("procon.protected.pluginconsole.write", "^1Metabans (api_mb_sight_player_ok): " + e.Message);
                }
            }
        }
        
        public override void OnPlayerAuthenticated(string soldierName, string guid) {
            if (this.Username.Length > 0 && this.ApiKey.Length > 0 && guid.Length > 0) {
                try {
                    if (this.Debug == enumBoolOnOff.On) {
                        this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans: Synchronizing sight of " + soldierName);
                    }

                    MetabansAPI api = new MetabansAPI(this.Username, this.ApiKey, this.Debug);
                    api.ExecuteCommand += new MetabansAPI.ExecuteCommandHandler(api_ExecuteCommand);
                    api.mb_sight_player_ok += new MetabansAPI.RequestSuccessHandler(api_mb_sight_player_ok);
                    
                    api.mb_sight_player(
                        #include "Metabans.GameType.inc",
                        guid,
                        soldierName,
                        this.ServerName,
                        null,
                        null
                    ).Post();
                }
                catch (Exception e) {
                    if (this.Debug == enumBoolOnOff.On) {
                        this.ExecuteCommand("procon.protected.pluginconsole.write", "Metabans (OnPlayerAuthenticated): " + e.Message);
                    }
                }
            }
        }
        
        public override void OnListPlayers(List<CPlayerInfo> players, CPlayerSubset subset) {
            base.OnListPlayers(players, subset);
            
            if (subset.Subset == CPlayerSubset.PlayerSubsetType.All) {
                if (LastSynced < DateTime.Now.AddMinutes(-2) && this.Username.Length > 0 && this.ApiKey.Length > 0) {
                
                    if (this.Debug == enumBoolOnOff.On) {
                        this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans: Synchronizing playerlist");
                    }
                
                    if (this.FrostbitePlayerInfoList.Count > 0) {
                    
                        try {
                            MetabansAPI api = new MetabansAPI(this.Username, this.ApiKey, this.Debug);
                            api.ExecuteCommand += new MetabansAPI.ExecuteCommandHandler(api_ExecuteCommand);
                            api.mb_sight_player_ok += new MetabansAPI.RequestSuccessHandler(api_mb_sight_player_ok);
                            
                            foreach (CPlayerInfo player in players) {
								if (player.GUID.Length > 0) {
									if (this.PunkbusterPlayerInfoList.ContainsKey(player.SoldierName) == true) {
										api.mb_sight_player(
											#include "Metabans.GameType.inc",
											player.GUID,
											player.SoldierName,
											this.ServerName,
											this.PunkbusterPlayerInfoList[player.SoldierName].Ip,
											this.PunkbusterPlayerInfoList[player.SoldierName].GUID
										);
									}
									else {
										api.mb_sight_player(
											#include "Metabans.GameType.inc",
											player.GUID,
											player.SoldierName,
											this.ServerName,
											null,
											null
										);
									}
								}
                            }
                            
                            api.Post();
                        }
                        catch (Exception e) {
                            this.ExecuteCommand("procon.protected.pluginconsole.write", "^1Metabans (OnListPlayers): " + e.Message);
                        }
                    }
                    else if (this.Debug == enumBoolOnOff.On) {
                        this.ExecuteCommand("procon.protected.pluginconsole.write", "^4Metabans: Playerlist empty, no need to nag metabans.");
                    }
                
                    LastSynced = DateTime.Now;
                }
            }
        }

		public static string GetDescriptionList<T>() {
			
			List<string> descriptions = new List<string>();

			foreach (T value in Enum.GetValues(typeof(T))) {
				descriptions.Add(Metabans.GetDescription<T>(value));
			}

			return String.Join("|", descriptions.ToArray());
		}

		// Taken and modified from http://weblogs.asp.net/grantbarrington/archive/2009/01/19/enumhelper-getting-a-friendly-description-from-an-enum.aspx
        public static string GetDescription<T>(T en) {
            Type type = typeof(T);

            MemberInfo[] memInfo = type.GetMember(en.ToString());

            if (memInfo != null && memInfo.Length > 0) {
                object[] attrs = memInfo[0].GetCustomAttributes(typeof(DescriptionAttribute), false);

                if (attrs != null && attrs.Length > 0) {
                    return ((DescriptionAttribute)attrs[0]).Description;
                }
            }

            return en.ToString();
        }

		public static T GetEnumFromDescription<T>(string description) {
			T returnValue = default(T);

			foreach (T value in Enum.GetValues(typeof(T))) {
				
				if (String.Compare(Metabans.GetDescription<T>(value), description) == 0) {
					returnValue = value;
				}

			}

			return returnValue;
		}

    }

	internal enum PropagationRules {
		[Description("Do not propagate my bans")]
		None,
		[Description("Send all bans to Metabans.com (default)")]
		All,
		[Description("Send bans if the reason contains one of the following words:")]
		Filtered
	}
    
	internal enum MetabansDurationTypes {
		[Description("Days")]
		Days,
		[Description("Months")]
		Months,
		[Description("Years")]
		Years
	}

    // This is where the story gets a little bit crazy, and a lot convoluted.
    // BFBC2/Metabans.cs - target directory is /BFBC2, thus we need to ../ our way back to
    // the Plugins folder where the API is located.
    
    #include "../Metabans.API.1_0_0_4.inc"
}











