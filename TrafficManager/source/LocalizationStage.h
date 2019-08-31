#pragma once

#include <memory>
#include <deque>

#include "carla/client/Actor.h"
#include "carla/geom/Vector3D.h"
#include "carla/geom/Location.h"
#include "carla/geom/Transform.h"
#include "carla/Memory.h"

#include "PipelineStage.h"
#include "SimpleWaypoint.h"
#include "Messenger.h"
#include "InMemoryMap.h"

namespace traffic_manager {

  struct MotionControlMessage {
    carla::SharedPtr<carla::client::Actor> actor;
    float deviation;
  };

  typedef std::vector<MotionControlMessage> MessageFrame;
  typedef Messenger<std::shared_ptr<MessageFrame>> MessengerType;

  class LocalizationStage : PipelineStage {

    /// This class is responsible of maintaining a horizon of waypoints ahead
    /// of the vehicle for it to follow.

  private:

    int motion_control_messenger_state;

    InMemoryMap& local_map;
    std::vector<carla::SharedPtr<carla::client::Actor>>& actor_list;
    std::vector<std::deque<std::shared_ptr<SimpleWaypoint>>> buffer_list;
    std::shared_ptr<MessengerType> motion_control_messenger;
    std::shared_ptr<MessageFrame> motion_control_frame;

    /// Returns the dot product between vehicle's heading vector and
    /// the vector along the direction to the next target waypoint in the
    /// horizon.
    float DeviationDotProduct(
        carla::SharedPtr<carla::client::Actor>,
        const carla::geom::Location &) const;

    /// Returns the cross product (z component value) between vehicle's heading
    /// vector and
    /// the vector along the direction to the next target waypoint in the
    /// horizon.
    float DeviationCrossProduct(
        carla::SharedPtr<carla::client::Actor>,
        const carla::geom::Location &) const;

  public:

    LocalizationStage(
      int pool_size,
      std::shared_ptr<MessengerType> motion_control_messenger,
      std::vector<carla::SharedPtr<carla::client::Actor>>& actor_list,
      InMemoryMap& local_map
    );

    ~LocalizationStage();

    void DataReceiver() override;
    void Action(int thread_id) override;
    void DataSender() override;

    using PipelineStage::Start;

  };

}