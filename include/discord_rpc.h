#pragma once
#include <stdint.h>

// clang-format off

#if defined(DISCORD_DYNAMIC_LIB)
#  if defined(_WIN32)
#    if defined(DISCORD_BUILDING_SDK)
#      define DISCORD_EXPORT __declspec(dllexport)
#    else
#      define DISCORD_EXPORT __declspec(dllimport)
#    endif
#  else
#    define DISCORD_EXPORT __attribute__((visibility("default")))
#  endif
#else
#  define DISCORD_EXPORT
#endif

// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

#define DISCORD_BUTTONS_SIZE 2

typedef struct DiscordButton {
    const char* label;
    const char* url;
} DiscordButton;

typedef enum DiscordActivityType {
    DiscordActivityType_Playing = 0,
    DiscordActivityType_Listening = 2,
    DiscordActivityType_Watching = 3,
    DiscordActivityType_Competing = 5
} DiscordActivityType;

typedef struct DiscordRichPresence {
    DiscordActivityType type;
    const char* state;
    const char* details;
    int64_t startTimestamp;
    int64_t endTimestamp;
    const char* largeImageKey;
    const char* largeImageText;
    const char* smallImageKey;
    const char* smallImageText;
    const char* partyId;
    int partySize;
    int partyMax;
    int partyPrivacy;
    const char* matchSecret;
    const char* joinSecret;
    const char* spectateSecret;
    int8_t instance;
    DiscordButton buttons[DISCORD_BUTTONS_SIZE];
} DiscordRichPresence;

typedef struct DiscordUser {
    const char* userId;
    const char* username;
    const char* discriminator;
    const char* avatar;
} DiscordUser;

typedef struct DiscordEventHandlers {
    void (*ready)(const DiscordUser* request);
    void (*disconnected)(int errorCode, const char* message);
    void (*errored)(int errorCode, const char* message);
    void (*joinGame)(const char* joinSecret);
    void (*spectateGame)(const char* spectateSecret);
    void (*joinRequest)(const DiscordUser* request);
} DiscordEventHandlers;

#define DISCORD_REPLY_NO 0
#define DISCORD_REPLY_YES 1
#define DISCORD_REPLY_IGNORE 2
#define DISCORD_PARTY_PRIVATE 0
#define DISCORD_PARTY_PUBLIC 1

DISCORD_EXPORT void Discord_Initialize(const char* applicationId,
                                       DiscordEventHandlers* handlers,
                                       int autoRegister,
                                       const char* optionalSteamId);
DISCORD_EXPORT bool Discord_Connected(void);
DISCORD_EXPORT void Discord_Shutdown(void);
DISCORD_EXPORT void Discord_RunCallbacks(void);
#ifdef DISCORD_DISABLE_IO_THREAD
DISCORD_EXPORT void Discord_UpdateConnection(void);
#endif
DISCORD_EXPORT void Discord_UpdatePresence(const DiscordRichPresence* presence);
DISCORD_EXPORT void Discord_ClearPresence(void);
DISCORD_EXPORT void Discord_Respond(const char* userid, int reply);
DISCORD_EXPORT void Discord_UpdateHandlers(DiscordEventHandlers* handlers);

#ifdef __cplusplus
}
#endif
