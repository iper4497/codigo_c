/*
 * Copyright (c) 2005-2009 Nominet UK (www.nic.uk)
 * All rights reserved.
 * Contributors: Ben Laurie, Rachel Willmer. The Contributors have asserted
 * their moral rights under the UK Copyright Design and Patents Act 1988 to
 * be recorded as the authors of this copyright work.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. 
 * 
 * You may obtain a copy of the License at 
 *     http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * 
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OPS_LITERAL_H__
#define __OPS_LITERAL_H__


ops_boolean_t write_literal_header(ops_create_info_t *info,
                                   void *header_data);

void ops_writer_push_literal(ops_create_info_t *info);
void ops_writer_push_literal_with_opts(ops_create_info_t *info,
				       unsigned int buf_size);

#endif /* __OPS_LITERAL_H__ */

// EOF
