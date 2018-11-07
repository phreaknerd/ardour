/*
    Copyright (C) 2017 Paul Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __gtk2_ardour_beatbox_gui_h__
#define __gtk2_ardour_beatbox_gui_h__

#include <string>
#include <boost/shared_ptr.hpp>

#include <gtkmm/radiobutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/button.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/box.h>
#include <gtkmm/notebook.h>

#include "gtkmm2ext/colors.h"

#include "widgets/ardour_button.h"

#include "canvas/box.h"
#include "canvas/canvas.h"
#include "canvas/rectangle.h"

#include "widgets/ardour_button.h"
#include "widgets/ardour_dropdown.h"
#include "ardour_dialog.h"

namespace PBD {
class PropertyChange;
}

namespace ArdourCanvas {
class Grid;
class Item;
class StepButton;
class Polygon;
class Text;
class VBox;
class Widget;
}

namespace ARDOUR {
class BeatBox;
class Step;
class StepSequencer;
}

class SequencerGrid;

class StepView : public ArdourCanvas::Rectangle, public sigc::trackable {
   public:
	StepView (SequencerGrid&, ARDOUR::Step&, ArdourCanvas::Item*);

	void render (ArdourCanvas::Rect const &, Cairo::RefPtr<Cairo::Context>) const;
	bool on_event (GdkEvent*);

	void view_mode_changed ();

   private:
	ARDOUR::Step& _step;
	SequencerGrid& _seq;
	ArdourCanvas::Text* text;

	std::pair<double,double> grab_at;

	bool motion_event (GdkEventMotion*);
	bool button_press_event (GdkEventButton*);
	bool button_release_event (GdkEventButton*);
	bool scroll_event (GdkEventScroll*);

	void adjust_step_pitch (int amt);
	void adjust_step_velocity (int amt);
	void adjust_step_duration (double);
	void adjust_step_octave (int amt);

	void step_changed (PBD::PropertyChange const &);
	PBD::ScopedConnection step_connection;

	void make_text ();
};

class SequencerGrid : public ArdourCanvas::Rectangle, public sigc::trackable {
  public:
	enum Mode {
		Velocity,
		Pitch,
		Duration,
		Octave,
		Group,
	};

	SequencerGrid (ARDOUR::StepSequencer&, ArdourCanvas::Canvas*);

	Mode mode() const { return _mode; }
	void set_mode (Mode m);

	void render (ArdourCanvas::Rect const &, Cairo::RefPtr<Cairo::Context>) const;

  private:
	ARDOUR::StepSequencer& _sequencer;
	typedef std::vector<StepView*> StepViews;
	StepViews _step_views;
	double _width;
	double _height;
	Mode   _mode;
	ArdourCanvas::ScrollGroup* v_scroll_group;
	ArdourCanvas::Container* no_scroll_group;
	ArdourCanvas::Rectangle* step_indicator_bg;
	ArdourCanvas::Container* step_indicator_box;

	void sequencer_changed (PBD::PropertyChange const &);

	PBD::ScopedConnection sequencer_connection;
};

class SequencerStepIndicator : public ArdourCanvas::Rectangle {
  public:
	SequencerStepIndicator (ArdourCanvas::Item *, int n);
	void render (ArdourCanvas::Rect const &, Cairo::RefPtr<Cairo::Context>) const;
  private:
	ArdourCanvas::Polygon* poly;
	ArdourCanvas::Text*    text;
};

class BBGUI : public ArdourDialog {
  public:
	BBGUI (boost::shared_ptr<ARDOUR::BeatBox> bb);
	~BBGUI ();

	double width() const { return _width; }
	double height() const { return _height; }

  protected:
	void on_map ();
	void on_unmap ();

  private:
	boost::shared_ptr<ARDOUR::BeatBox> bbox;
	double _width;
	double _height;

	Gtk::Adjustment horizontal_adjustment;
	Gtk::Adjustment vertical_adjustment;

	ArdourCanvas::GtkCanvasViewport* _canvas_viewport;
	ArdourCanvas::GtkCanvas* _canvas;

	SequencerGrid* _sequencer;

	ArdourWidgets::ArdourButton start_button;
	void toggle_play ();

	ArdourWidgets::ArdourButton export_as_region_button;
	void export_as_region ();

	Gtk::HBox canvas_hbox;
	Gtk::VScrollbar vscrollbar;

	void clear ();
	void update ();
	void update_sequencer ();

	sigc::connection timer_connection;

	void sequencer_changed (PBD::PropertyChange const &);

	Gtk::HBox mode_box;
	ArdourWidgets::ArdourButton mode_velocity_button;
	ArdourWidgets:: ArdourButton mode_pitch_button;
	ArdourWidgets::ArdourButton mode_octave_button;
	ArdourWidgets::ArdourButton mode_group_button;
	ArdourWidgets::ArdourButton mode_duration_button;

	void mode_clicked (SequencerGrid::Mode);

	PBD::ScopedConnection sequencer_connection;
};

#endif /* __gtk2_ardour_beatbox_gui_h__ */
