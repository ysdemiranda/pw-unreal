#pragma once

#include "CoreMinimal.h"
#include "pipewire/pipewire.h"
#include "Roundtrip.generated.h"

UCLASS(Blueprintable, BlueprintType, Category = "Pipewire")
class UNREALPW_API URoundtrip : public UObject {
    GENERATED_BODY()

    struct roundtrip_data {
        int pending;
        struct pw_main_loop *loop;
    };

    static void on_core_done(void *data, uint32_t id, int seq);
    static void roundtrip(struct pw_core *core, struct pw_main_loop *loop);
    static void registry_event_global(void *data, uint32_t id,
        uint32_t permissions, const char *type, uint32_t version,
        const struct spa_dict *props);

    static const struct pw_registry_events constexpr registry_events = {
        .version = PW_VERSION_REGISTRY_EVENTS,
        .global = registry_event_global
    };

    public:
    UFUNCTION(BlueprintCallable, Category = "PipeWire|Core")
    void Run();
};