datadistfiles = default/mod_def \
default/ground \
default/game \
default/env \
default/cursors/def \
default/cursors/c_normal.png \
default/cursors/c_bottom.png \
default/cursors/c_bottom2.png \
default/cursors/c_bottom3.png \
default/cursors/c_left.png \
default/cursors/c_left2.png \
default/cursors/c_left3.png \
default/cursors/c_right.png \
default/cursors/c_right2.png \
default/cursors/c_right3.png \
default/cursors/c_top.png \
default/cursors/c_top2.png \
default/cursors/c_top3.png \
default/cursors/c_topleft.png \
default/cursors/c_topleft2.png \
default/cursors/c_topleft3.png \
default/cursors/c_topright.png \
default/cursors/c_topright2.png \
default/cursors/c_topright3.png \
default/cursors/c_bottomright.png \
default/cursors/c_bottomright2.png \
default/cursors/c_bottomright3.png \
default/cursors/c_bottomleft.png \
default/cursors/c_bottomleft2.png \
default/cursors/c_bottomleft3.png \
default/pict/loading.png \
default/pict/sel_frame.png \
default/fonts/default8.png \
default/fonts/def \
default/fonts/default8 \
default/fonts/default12.png \
default/fonts/default12 \
default/fonts/default16.png \
default/fonts/default16 \
default/fonts/opt_menubar \
default/fonts/opt_shell \
default/meshes/skybox \
default/meshes/bridge \
default/meshes/discthrower \
default/meshes/suncannon \
default/meshes/generator \
default/meshes/temple \
default/meshes/workshop \
default/gui/sidepanel \
default/gui/menubar \
default/gui/def \
default/gui/shell \
default/gui/map \
default/gui/pict/menubar_bg \
default/gui/pict/menubar_bg.png \
default/gui/pict/sidepanel_bg \
default/gui/pict/sidepanel_bg.png \
default/gui/pict/menu_bg \
default/gui/pict/menu_bg.png \
default/gui/pict/shellbg.png \
default/gui/pict/shellbgbox.png \
default/gui/pict/buttonbg.png \
default/gui/pict/buttonpushed.png \
default/gui/pict/tooltipbg.png \
default/gui/pict/scrollbarbg.png \
default/gui/pict/dialogbg.png \
default/gui/icons/bridge.png \
default/gui/icons/generator.png \
default/gui/icons/discthrower.png \
default/gui/icons/suncannon.png \
default/gui/icons/temple.png \
default/gui/icons/workshop.png \
default/gui/menus/game \
default/gui/menus/options \
default/gui/menus/optiongamma \
default/gui/menus/optionres \
default/gui/menus/optionfps \
default/gui/menus/optiontex \
default/i18n/def \
default/i18n/fr \
default/i18n/pt \
default/i18n/pl \
default/tex/def \
default/tex/sb_top.png \
default/tex/sb_bottom.png \
default/tex/sb_left.png \
default/tex/sb_right.png \
default/tex/sb_front.png \
default/tex/sb_back.png \
default/tex/range.png \
default/tex/grass.png \
default/tex/bridge_top.png \
default/tex/bridge_side.png \
default/tex/cloud1.png \
default/tex/soil.png \
default/tex/static_base.png \
default/tex/discthrower_arm.png \
default/tex/discthrower_body.png \
default/tex/discthrower_holder.png \
default/tex/suncannon_back.png \
default/tex/suncannon_cannon.png \
default/tex/suncannon_cord.png \
default/tex/suncannon_holder.png \
default/tex/suncannon_panel.png \
default/tex/suncannon_ring.png \
default/tex/generator_base1.png \
default/tex/generator_base2.png \
default/tex/generator_crystal.png \
default/tex/generator_pads.png \
default/tex/generator_pylons.png \
default/tex/workshop_base.png \
default/tex/workshop_ss.png \
default/tex/temple_rim.png \
default/tex/temple_center.png \
default/tex/temple_pipes.png \
default/tex/temple_ring1.png \
default/tex/temple_ring2.png \
default/tex/temple_tornado.png \
default/tex/temple_tornadofeet.png \
default/tex/black.png \
default/tex/white.png \
default/music/def \
default/music/music.ogg \
default/sound/def \
default/sound/thunder01.ogg \
default/sound/sp_select.ogg \
default/sound/sp_unselect.ogg \
stormwar_data

datadistdirs = default/cursors \
	default/fonts \
	default/i18n \
	default/pict \
    default/tex \
    default/meshes \
    default/music \
    default/sound \
    default/gui/pict \
    default/gui/icons \
    default/gui/menus \
    default/gui \
    default

uninstall-hook:
	for dir in $(datadistdirs); do rmdir "$(datadir)/$(PACKAGE)/$$dir"; done
	rmdir "$(datadir)/$(PACKAGE)"

EXTRA_DIST = $(datadistfiles)
nobase_dist_pkgdata_DATA = $(EXTRA_DIST)
