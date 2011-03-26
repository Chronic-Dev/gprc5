ifeq ($(_THEOS_RULES_LOADED),)
_THEOS_RULES_LOADED := 1

ifeq ($(THEOS_CURRENT_INSTANCE),)
	include $(THEOS_MAKE_PATH)/master/rules.mk
	-include $(foreach mod,$(_THEOS_LOAD_MODULES),$(THEOS_MODULE_PATH)/$(mod)/master/rules.mk)
else
	include $(THEOS_MAKE_PATH)/instance/rules.mk
	-include $(foreach mod,$(_THEOS_LOAD_MODULES),$(THEOS_MODULE_PATH)/$(mod)/instance/rules.mk)
endif

ALL_CFLAGS = $(INTERNAL_CFLAGS) $(TARGET_CFLAGS) $(ADDITIONAL_CFLAGS) $(AUXILIARY_CFLAGS) $(DEBUG_CFLAGS) $(CFLAGS)
ALL_CCFLAGS = $(ADDITIONAL_CCFLAGS) $(CCFLAGS)
ALL_OBJCFLAGS = $(INTERNAL_OBJCFLAGS) $(TARGET_OBJCFLAGS) $(ADDITIONAL_OBJCFLAGS) $(AUXILIARY_OBJCFLAGS) $(DEBUG_CFLAGS) $(OBJCFLAGS)
ALL_OBJCCFLAGS = $(ADDITIONAL_OBJCCFLAGS) $(OBJCCFLAGS)

ALL_LDFLAGS = $(INTERNAL_LDFLAGS) $(ADDITIONAL_LDFLAGS) $(AUXILIARY_LDFLAGS) $(TARGET_LDFLAGS) $(DEBUG_LDFLAGS) $(LDFLAGS)

ifeq ($(_THEOS_MAKE_PARALLEL_BUILDING), no)
.NOTPARALLEL:
else
ifneq ($(_THEOS_MAKE_PARALLEL), yes)
.NOTPARALLEL:
endif
endif

.SUFFIXES:

.SUFFIXES: .m .mm .c .cc .cpp .xm

$(THEOS_OBJ_DIR)/%.m.o: %.m
	$(ECHO_COMPILING)$(TARGET_CXX) -x objective-c -c $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(TARGET_ONLY_OBJCFLAGS) $< -o $@$(ECHO_END)

$(THEOS_OBJ_DIR)/%.mm.o: %.mm
	$(ECHO_COMPILING)$(TARGET_CXX) -c $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(ALL_CCFLAGS) $(ALL_OBJCCFLAGS) $< -o $@$(ECHO_END)

$(THEOS_OBJ_DIR)/%.c.o: %.c
	$(ECHO_COMPILING)$(TARGET_CXX) -x c -c $(ALL_CFLAGS) $< -o $@$(ECHO_END)

$(THEOS_OBJ_DIR)/%.cc.o: %.cc
	$(ECHO_COMPILING)$(TARGET_CXX) -c $(ALL_CFLAGS) $(ALL_CCFLAGS) $< -o $@$(ECHO_END)

$(THEOS_OBJ_DIR)/%.cpp.o: %.cpp
	$(ECHO_COMPILING)$(TARGET_CXX) -c $(ALL_CFLAGS) $(ALL_CCFLAGS) $< -o $@$(ECHO_END)

$(THEOS_OBJ_DIR)/%.x.o: %.x
	$(ECHO_PREPROCESSING)$(THEOS_BIN_PATH)/logos.pl $< > $(THEOS_OBJ_DIR)/$<.m$(ECHO_END)
	$(ECHO_COMPILING)$(TARGET_CXX) -x objective-c -c -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(TARGET_ONLY_OBJCFLAGS) $(THEOS_OBJ_DIR)/$<.m -o $@$(ECHO_END)
	$(ECHO_NOTHING)rm $(THEOS_OBJ_DIR)/$<.m$(ECHO_END)

$(THEOS_OBJ_DIR)/%.xm.o: %.xm
	$(ECHO_PREPROCESSING)$(THEOS_BIN_PATH)/logos.pl $< > $(THEOS_OBJ_DIR)/$<.mm$(ECHO_END)
	$(ECHO_COMPILING)$(TARGET_CXX) -x objective-c++ -c -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(ALL_CCFLAGS) $(ALL_OBJCCFLAGS) $(THEOS_OBJ_DIR)/$<.mm -o $@$(ECHO_END)
	$(ECHO_NOTHING)rm $(THEOS_OBJ_DIR)/$<.mm$(ECHO_END)

$(THEOS_OBJ_DIR)/%.xi.o: %.xi
	$(ECHO_PREPROCESSING)$(TARGET_CXX) -x objective-c -E -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(TARGET_ONLY_OBJCFLAGS) -include substrate.h $< > $(THEOS_OBJ_DIR)/$<.pre && $(THEOS_BIN_PATH)/logos.pl $(THEOS_OBJ_DIR)/$<.pre > $(THEOS_OBJ_DIR)/$<.mi $(ECHO_END)
	$(ECHO_COMPILING)$(TARGET_CXX) -c -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(TARGET_ONLY_OBJCFLAGS) $(THEOS_OBJ_DIR)/$<.mi -o $@$(ECHO_END)
	$(ECHO_NOTHING)rm $(THEOS_OBJ_DIR)/$<.pre $(THEOS_OBJ_DIR)/$<.mi$(ECHO_END)

$(THEOS_OBJ_DIR)/%.xmi.o: %.xmi
	$(ECHO_PREPROCESSING)$(TARGET_CXX) -x objective-c++ -E -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(ALL_CCFLAGS) $(ALL_OBJCCFLAGS) -include substrate.h $< > $(THEOS_OBJ_DIR)/$<.pre && $(THEOS_BIN_PATH)/logos.pl $(THEOS_OBJ_DIR)/$<.pre > $(THEOS_OBJ_DIR)/$<.mii $(ECHO_END)
	$(ECHO_COMPILING)$(TARGET_CXX) -c -I"$(shell pwd)" $(ALL_CFLAGS) $(ALL_OBJCFLAGS) $(ALL_CCFLAGS) $(ALL_OBJCCFLAGS) $(THEOS_OBJ_DIR)/$<.mii -o $@$(ECHO_END)
	$(ECHO_NOTHING)rm $(THEOS_OBJ_DIR)/$<.pre $(THEOS_OBJ_DIR)/$<.mii$(ECHO_END)

%.mm: %.l.mm
	$(THEOS_BIN_PATH)/logos.pl $< > $@

%.mm: %.xmm
	$(THEOS_BIN_PATH)/logos.pl $< > $@

%.mm: %.xm
	$(THEOS_BIN_PATH)/logos.pl $< > $@

%.m: %.xm
	$(THEOS_BIN_PATH)/logos.pl $< > $@

ifneq ($(THEOS_BUILD_DIR),.)
$(THEOS_BUILD_DIR):
	@mkdir -p $(THEOS_BUILD_DIR)
endif

$(THEOS_OBJ_DIR):
	@cd $(THEOS_BUILD_DIR); mkdir -p $(THEOS_OBJ_DIR_NAME)

$(THEOS_OBJ_DIR)/.stamp: $(THEOS_OBJ_DIR)
	@touch $@

$(THEOS_OBJ_DIR)/%/.stamp: $(THEOS_OBJ_DIR)
	@mkdir -p $(dir $@); touch $@

Makefile: ;
$(_THEOS_RELATIVE_MAKE_PATH)%.mk: ;
$(THEOS_MAKE_PATH)/%.mk: ;
$(THEOS_MAKE_PATH)/master/%.mk: ;
$(THEOS_MAKE_PATH)/instance/%.mk: ;
$(THEOS_MAKE_PATH)/instance/shared/%.mk: ;
$(THEOS_MAKE_PATH)/platform/%.mk: ;
$(THEOS_MAKE_PATH)/targets/%.mk: ;
$(THEOS_MAKE_PATH)/targets/%/%.mk: ;

ifneq ($(THEOS_PACKAGE_DIR_NAME),)
$(THEOS_PACKAGE_DIR):
	@cd $(THEOS_BUILD_DIR); mkdir -p $(THEOS_PACKAGE_DIR_NAME)
endif

endif

# TODO MAKE A BUNCH OF THINGS PHONY
-include $(foreach mod,$(_THEOS_LOAD_MODULES),$(THEOS_MODULE_PATH)/$(mod)/rules.mk)
