// view_manager.h — owns the views and runs the active one.
#pragma once

#include <stdint.h>

#include "face_view.h"
#include "image_view.h"
#include "text_view.h"
#include "vibe_test_view.h"

class Renderer;

// ViewManager owns the FaceView, TextView and ImageView and tracks which
// is active. setView() drives the onExit/onEnter handoff; tick() advances
// and draws the active view once per loop.
class ViewManager {
 public:
  void setView(View* v);
  void tick(uint32_t now, Renderer& r);

  FaceView& face() { return face_; }
  TextView& text() { return text_; }
  ImageView& image() { return image_; }
  VibeTestView& vibeTest() { return vibeTest_; }

  bool isFaceActive() const { return active_ == &face_; }
  bool isTextActive() const { return active_ == &text_; }
  bool isImageActive() const { return active_ == &image_; }
  bool isVibeTestActive() const { return active_ == &vibeTest_; }

  // "face", "text", "image", "vibetest", or "none" — used by GET state.
  const char* activeViewName() const;

 private:
  FaceView face_;
  TextView text_;
  ImageView image_;
  VibeTestView vibeTest_;
  View* active_ = nullptr;
};
