#pragma once
class SCC;
class SCC_IOP;
std::shared_ptr<SCC> newZ8530( bool is_iop = false);
std::shared_ptr<SCC> newZ85C80( bool is_iop = false);
