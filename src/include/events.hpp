#pragma once

#include <functional>
#include <vector>
#include <memory>

typedef std::function<void(std::string)> EventHandler;

/// @brief a particular event
typedef struct {
    std::vector<EventHandler> handlers;
} Event;

/// @brief init event
/// @param event 
static void InitEvent(Event *event){
    event->handlers = std::vector<EventHandler>();
}

/// @brief add an event handler to an event
/// @param event 
/// @param handler 
void AddEventHandler(Event& event, const EventHandler handler) {
    event.handlers.emplace_back(handler);
}

/// @brief broadcast the event to all listeners
/// @param event 
static void EmitEvent(Event& event, std::string returnContent){
    for (int i = 0; i < event.handlers.size(); i++) {
        event.handlers[i](returnContent);
    }
}

static void ClearEvents(std::shared_ptr<Event> event){
    event.reset();
}