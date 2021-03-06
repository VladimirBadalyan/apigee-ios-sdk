/*
 * Copyright (c) 2013 Plausible Labs Cooperative, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "apigee_dwarf_opstream.hpp"

#include "ApigeePLCrashFeatureConfig.h"

#if PLCRASH_FEATURE_UNWIND_DWARF

namespace apigee { namespace plcrash { namespace async {

/**
 * @internal
 * @ingroup plcrash_async_dwarf_private
 * @defgroup plcrash_async_dwarf_private_opstream Generic DWARF Opcode Stream
 * @{
 */

/**
 * Initialize the DWARF opcode stream.
 *
 * @param mobj The memory object from which the expression opcodes will be read. This object must
 * remain valid for the lifetime of the opstream instance.
 * @param byteorder The byte order of the data referenced by @a mobj and @a thread_state.
 * @param address The task-relative address within @a mobj at which the opcodes will be fetched.
 * @param offset An offset to be applied to @a address.
 * @param length The total length of the opcodes readable at @a address + @a offset.
 */
apigee_plcrash_error_t dwarf_opstream::init (apigee_plcrash_async_mobject_t *mobj,
                                      const apigee_plcrash_async_byteorder_t *byteorder,
                                      pl_vm_address_t address,
                                      pl_vm_off_t offset,
                                      pl_vm_size_t length)
{
    _mobj = mobj;
    _byteorder = byteorder;
    
    /* Calculate the start and end addresses */
    if (!apigee_plcrash_async_address_apply_offset(address, offset, &_start)) {
        PLCF_DEBUG("Offset overflows base address");
        return APIGEE_PLCRASH_EINVAL;
    }
    
    if (length > PL_VM_OFF_MAX || !apigee_plcrash_async_address_apply_offset(_start, length, &_end)) {
        PLCF_DEBUG("Length overflows base address");
        return APIGEE_PLCRASH_EINVAL;
    }
    
    /* Map in the full instruction range */
    _instr = apigee_plcrash_async_mobject_remap_address(mobj, _start, 0, _end-_start);
    _instr_max = (uint8_t *)_instr + (_end - _start);
    _p = _instr;
    
    if (_instr == NULL) {
        PLCF_DEBUG("Could not map the DWARF instructions; range falls outside mapped pages");
        return APIGEE_PLCRASH_EINVAL;
    }
    
    return APIGEE_PLCRASH_ESUCCESS;
}


/**
 * @}
 */

}}}

#endif /* PLCRASH_FEATURE_UNWIND_DWARF */
