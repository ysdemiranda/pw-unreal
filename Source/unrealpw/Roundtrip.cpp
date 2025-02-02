#include "Roundtrip.h"

#include "Public/unrealpw.h"

void URoundtrip::on_core_done(void *data, uint32_t id, int seq)
{
    struct roundtrip_data *d = (roundtrip_data*)data;

    if(id == PW_ID_CORE && seq == d->pending)
        pw_main_loop_quit(d->loop);
}

void URoundtrip::roundtrip(struct pw_core *core, struct pw_main_loop *loop)
{
    static const struct pw_core_events core_events = {
        .version = PW_VERSION_CORE_EVENTS,
        .done = on_core_done
    };

    struct roundtrip_data d = { .loop = loop };
    struct spa_hook core_listener;
    int err;

    pw_core_add_listener(core, &core_listener, &core_events, &d);

    d.pending = pw_core_sync(core, PW_ID_CORE, 0);
    if((err = pw_main_loop_run(loop)) < 0 )
        UE_LOG(LogPipeWire, Error, TEXT("main_loop_run error:\t%d"), err);

    spa_hook_remove(&core_listener);
}

void URoundtrip::registry_event_global(void *data, uint32_t id,
    uint32_t permissions, const char *type, uint32_t version,
    const struct spa_dict *props)
{
    UE_LOG(LogPipeWire, Display, TEXT("pw_object id:%u\ttype:%hs\tversion:%d\t"), id, type, version);
}

void URoundtrip::Run()
{
    struct pw_main_loop *loop;
    struct pw_context *context;
    struct pw_core *core;
    struct pw_registry *registry;
    struct spa_hook registry_listener;

    loop = pw_main_loop_new(NULL);
    context = pw_context_new(
        pw_main_loop_get_loop(loop),
        NULL,
        0);
    core = pw_context_connect(
        context,
        NULL,
        0);
    registry = pw_core_get_registry(
        core,
        PW_VERSION_REGISTRY,
        0);

    pw_registry_add_listener(
        registry, 
        &registry_listener,
        &registry_events,
        NULL);

        roundtrip(core, loop);

        pw_proxy_destroy((struct pw_proxy*)registry);
        pw_core_disconnect(core);
        pw_context_destroy(context);
        pw_main_loop_destroy(loop);
}