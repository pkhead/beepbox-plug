#include "device.hpp"

#include "system.hpp"

using namespace bbxplug;

clap_process_status device::process(const clap_process_t *process) {
    clap_process_status retval;
    denormal_disable_guard ddguard{};

    // if (plug->gui)
    //     plugin_process_gui_events(plug, process->out_events);

    if (process->transport) process_transport(process->transport);

    // process internal events sent from main (i.e. from state loading)
    // but do not process internal events while waiting for a synth type change
    // to finalize.
    // for (internal_event_queue_item_s ev;
    //     !plugin_waiting_for_synth_change(plug) && plugin_dequeue_event(plug,
    //     &ev);) { plugin_process_internal_event(plug, &ev, SEND_TO_HOST,
    //                                   process->out_events);
    //}

    const uint32_t nframes = process->frames_count;
    const uint32_t nev = process->in_events->size(process->in_events);
    uint32_t ev_index = 0;
    uint32_t next_ev_frame = nev > 0 ? 0 : nframes;

    clap_process_status stat = CLAP_PROCESS_CONTINUE;
    for (uint32_t i = 0; i < nframes;) {
        // handle every events that happrens at the frame "i"
        while (ev_index < nev && next_ev_frame == i) {
            const clap_event_header_t *hdr =
                process->in_events->get(process->in_events, ev_index);
            if (hdr->time != i) {
                next_ev_frame = hdr->time;
                break;
            }

            process_event(hdr, process->out_events);
            ++ev_index;

            if (ev_index == nev) {
                // we reached the end of the event list
                next_ev_frame = nframes;
                break;
            }
        }

        // process every samples until the next event
        uint32_t frame_count = next_ev_frame - i;

        float *output[2];
        output[0] = process->audio_outputs->data32[0] + i;
        output[1] = process->audio_outputs->data32[1] + i;
        stat = run(output, frame_count, i, process->out_events);

        switch (stat) {
        case CLAP_PROCESS_ERROR:
            return CLAP_PROCESS_ERROR;
        default:
            break;
        }

        i += frame_count;
    }

    return stat;
}