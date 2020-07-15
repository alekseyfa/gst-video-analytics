/*******************************************************************************
<<<<<<< HEAD
 * Copyright (C) 2018-2020 Intel Corporation
=======
 * Copyright (C) 2018-2019 Intel Corporation
>>>>>>> 1bdbbc8... Release 2020.4
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#pragma once

#include <gst/gst.h>

enum class TrackingTerm { Long, Short, Zero };

struct ITracker {
    virtual ~ITracker() = default;
    virtual void track(GstBuffer *buffer) = 0;
};
