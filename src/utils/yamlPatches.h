/**
 * @file yamlPatches.h
 *
 * This file contains patches that should be added to yaml-cpp,
 * but haven't been merged for various reasons.
 * We keep them in a separate file so that, once all patches are merged,
 * this file can be deleted without affecting other templates.
 */

#pragma once

#include <yaml-cpp/yaml.h>

#include "Settings.h"

namespace YAML {

/**
 * @brief Support std::array as a sequential container
 *
 * This allows one to redirect arrays into a emitter
 *
 * @tparam T
 * @tparam N
 * @param emitter
 * @param v
 * @return Emitter&
 *
 * @see https://github.com/jbeder/yaml-cpp/issues/877
 */
template <typename T, size_t N>
inline Emitter &operator<<(Emitter &emitter, const std::array<T, N> &v) {
  return EmitSeq(emitter, v);
}

}  // namespace YAML
