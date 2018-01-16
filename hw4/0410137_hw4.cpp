#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <vector>

using namespace std;

struct TLB_entry
{
	int page_num;
	int frame_num;
	int cnt;
};

bool LRU(TLB_entry a, TLB_entry b)
{
	return a.cnt < b.cnt;
}

int main(int argc, char** argv)
{
	/*
	* BACK_STORE: 256 frames, frame size is 256 Byte
	* addr_in: n logical addresses need to be transferred
	*/
	FILE* BACK_STORE;
	BACK_STORE = fopen(argv[1], "rb");
	ifstream addr_in(argv[2]);
	
	if (!BACK_STORE || !addr_in){
		perror ("Error opening file");
		exit (EXIT_FAILURE);
	}
	else {
		int timer = 0;
		// data for TLB, page table, and physical memory
		vector<int> page_table;
		vector<TLB_entry> TLB;
		vector<char> physical_memory[256];
		
		for(int i=0; i<256; ++i){
			page_table.push_back(-1);
		}
		
		TLB_entry temp;
		for(int i=0; i<16; ++i) {
			temp.page_num = 0;
			temp.frame_num = 0;
			temp.cnt = 0;
			TLB.push_back(temp);
		}
		
		// data for input and output
		int n_addr;
		int *logical_addr;
		int *physical_addr;
		int *data;
		
		int TLB_hit = 0;
		int page_fault = 0;
		
		addr_in >> n_addr;
		logical_addr = new int[n_addr];
		physical_addr = new int[n_addr];
		data = new int[n_addr];
		
		for(int i=0; i<n_addr; ++i) {
			addr_in >> logical_addr[i];
		}
		addr_in.close();

		// find out all physical address
		int page, offset, frame;
		bool in_TLB;
		char read_data[256];
		for(int i=0; i<n_addr; ++i){
			in_TLB = false;
			++timer;
			page = (logical_addr[i]>>8);
			offset = (logical_addr[i] & 0xFF);
			
			// first find in TLB
			for(int j=0; j<16; ++j){
				if((page==TLB[j].page_num) && TLB[j].cnt){
					in_TLB = true;
					++TLB_hit;
					frame = TLB[j].frame_num;
					physical_addr[i] = (frame<<8)+offset;
					data[i] = physical_memory[frame][offset];
					TLB[j].cnt = timer;
					break;
				}
			}
			
			// TLB miss, find in page table
			if (!in_TLB){
				// page fault occurs
				if (page_table[page]==-1){
					// find frame number in BACK_STORE: seek then extract
					fseek(BACK_STORE, page<<8, SEEK_SET); 
					fread(read_data, sizeof(char), 256, BACK_STORE);

					// read data into physical memory (use # of page faults as frame number)
					for(int j=0; j<256; ++j){
						physical_memory[page_fault].push_back(read_data[j]);
					}
					
					// update page table
					page_table[page] = page_fault;
					frame = page_table[page];
					
					// output data of physical address and data in it
					physical_addr[i] = (frame<<8)+offset;
					data[i] = physical_memory[frame][offset];
					
					// update TLB
					stable_sort(TLB.begin(), TLB.end(), LRU);
					TLB[0].page_num = page;
					TLB[0].frame_num = frame;
					TLB[0].cnt = timer;
					
					++page_fault;
				} else { // frame number is in page table
					frame = page_table[page];
					physical_addr[i] = (frame<<8)+offset;
					data[i] = physical_memory[frame][offset];
				}
			}
		}
		fclose(BACK_STORE);
		
		// output results
		ofstream output("results.txt");
		for (int i=0; i<n_addr; ++i){
			output << physical_addr[i] << " " << data[i] << endl;
		}
		output << "TLB hits: " << TLB_hit << endl;
		output << "Page Faults: " << page_fault << endl;
		output.close();
		
		delete[] logical_addr;
		delete[] physical_addr;
		delete[] data;
	}
	
	return 0;
}