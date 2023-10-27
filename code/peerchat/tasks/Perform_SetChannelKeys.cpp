#include "tasks.h"
#include <sstream>
#include <server/Server.h>

namespace Peerchat {

	bool Perform_SetChannelKeys(PeerchatBackendRequest request, TaskThreadData* thread_data) {
		TaskResponse response;

		std::map<std::string, std::string> broadcast_keys;
		ChannelSummary summary = GetChannelSummaryByName(thread_data, request.channel_summary.channel_name, false);

		std::ostringstream ss;
		ss << "channel_" << summary.channel_id << "_custkeys";

		std::string channel_key = ss.str();

		if (summary.channel_id != 0) {
			response.error_details.response_code = TaskShared::WebErrorCode_Success;

			int from_mode_flags = LookupUserChannelModeFlags(thread_data, summary.channel_id, request.peer->GetBackendId());
			if (!CheckActionPermissions(request.peer, request.channel_summary.channel_name, from_mode_flags, 0, (int)EUserChannelFlag_Op)) {
				
				response.channel_summary = summary;
				if (request.callback) {
					request.callback(response, request.peer);
				}
				if (request.peer) {
					request.peer->DecRef();
				}
				return true;
				
			}
			if (request.peer->GetChannelFlags(summary.channel_id) & EUserChannelFlag_IsInChannel) {
				std::pair<std::vector<std::pair< std::string, std::string> >::const_iterator, std::vector<std::pair< std::string, std::string> >::const_iterator> iterators = request.channel_modify.kv_data.GetHead();
				std::vector<std::pair< std::string, std::string> >::const_iterator it = iterators.first;
				while (it != iterators.second) {
					std::pair<std::string, std::string> p = *it++;
					if(Is_Chankey_InjectKey(p.first.c_str())) {
						continue;
					}
					if (p.first.length() > 2 && p.first.substr(0, 2).compare("b_") == 0) {
						broadcast_keys[p.first] = p.second;
					}

					ss.str("");
					ss << p.first;
					std::string custkey_key = ss.str();

					
					void *reply = (redisReply *)redisCommand(thread_data->mp_redis_connection, "HSET %s %s %s", channel_key.c_str(), p.first.c_str(), p.second.c_str());
					freeReplyObject(reply);
				}


				if (broadcast_keys.size() > 0) {
					std::string kv_string = OS::MapToKVString(broadcast_keys);

					const char* base64 = OS::BinToBase64Str((uint8_t*)kv_string.c_str(), kv_string.length());
					std::ostringstream message;
					message << "\\type\\SETCHANKEY\\channel_id\\" << summary.channel_id << "\\keys\\" << base64;
					sendAMQPMessage(peerchat_channel_exchange, peerchat_key_updates_routingkey, message.str().c_str());

					free((void*)base64);
				}
			}
			else { //not in channel
				response.error_details.response_code = TaskShared::WebErrorCode_NoSuchUser;
			}
		}
		else { //no such channel
			response.error_details.response_code = TaskShared::WebErrorCode_NoSuchUser;
		}

		


		response.channel_summary = summary;
		if (request.callback) {
			request.callback(response, request.peer);
		}
		if (request.peer) {
			request.peer->DecRef();
		}
		return true;
	}
}
