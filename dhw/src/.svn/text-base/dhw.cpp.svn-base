#include "dhw.h"

struct DWH_NODE {
	short last_ref, last_mch;
	int err;
};

static void d_accsum(const int* hist, int len, int* Hist)
/**
	Hist[i] = sum_{n = 0}^{i-1} hist[n]
*/
{
	Hist[0] = 0;
	int i;
	for (i = 0; i < len; i ++)
		Hist[i + 1] = Hist[i] + hist[i];
}

const int ERR_INF = 0x6fffffff;

static void d_match_hist(const int* hist_ref, const int* hist_mch, BYTE* conv_ref, BYTE* conv_mch)
/**
	D(0, 0) = 0
	D(i, j) = \infnty, (i <= 0, j <= 0, (i, j) != (0, 0))
	D(m, n) = min \begin{cases}
					D(m - 1, n - 1) + d_{1, 1}(m, n)
					D(m - k, n - 1) + d_{k, 1}(m, n), 2 <= k <= m
					D(m - 1, n - l) + d_{1, l}(m, n), 2 <= l <= n
	              \end{cases}
	d_{k, l}(m, n) = |(H^A_m - H^A_{m-k}) -  (H^B_n - H^B_{n-l})|
*/
{
	static DWH_NODE nodes[257][257];

	static int H_ref[257], H_mch[257];
	d_accsum(hist_ref, 256, H_ref);
	d_accsum(hist_mch, 256, H_mch);

	// a macro for d_{k, l}(m, n)
#define	DWH_d(k, l, m, n)	(abs((H_ref[(m)] - H_ref[(m)-(k)]) - (H_mch[(n)] - H_mch[(n)-(l)])))

	const int M = 2;	// maximum allowable compression of ref histogram
	const int N = 2;	// maximum allowable compression of mch histogram

	// border values
	nodes[0][0].err = 0;

	int i;
	for (i = 1; i <= 256; i ++) {
		nodes[i][0].err = ERR_INF;
		nodes[0][i].err = ERR_INF;
	} // for i

	// Dynamic programming
	int idx_ref, idx_mch;
	for (idx_ref = 1; idx_ref <= 256; idx_ref ++) {
		for (idx_mch = 1; idx_mch <= 256; idx_mch ++) {
			if (idx_mch + idx_ref == 0)
				continue;

			// D(m - 1, n - 1) + d_{1, 1}(m, n)
			int err_min = nodes[idx_ref - 1][idx_mch - 1].err + DWH_d(1, 1, idx_ref, idx_mch);
			int last_ref = idx_ref - 1;
			int last_mch = idx_mch - 1;

			// D(m - k, n - 1) + d_{k, 1}(m, n), 2 <= k <= m
			int k;
			for (k = 2; k <= idx_ref; k ++) {
				if (k > N)
					break;
				const int err_cur = nodes[idx_ref - k][idx_mch - 1].err + DWH_d(k, 1, idx_ref, idx_mch);
				if (err_cur < err_min) {
					err_min = err_cur;
					last_ref = idx_ref - k;
					last_mch = idx_mch - 1;
				} // if
			} // for k

			// D(m - 1, n - l) + d_{1, l}(m, n), 2 <= l <= n
			int l;
			for (l = 2; l <= idx_mch; l ++) {
				if (l > N)
					break;
				const int err_cur = nodes[idx_ref - 1][idx_mch - l].err + DWH_d(1, l, idx_ref, idx_mch);
				if (err_cur < err_min) {
					err_min = err_cur;
					last_ref = idx_ref - 1;
					last_mch = idx_mch - l;
				} // if
			} // for l

			// store best one
			nodes[idx_ref][idx_mch].err = err_min;
			nodes[idx_ref][idx_mch].last_ref = last_ref;
			nodes[idx_ref][idx_mch].last_mch = last_mch;
		} // for idx_mch
	} // for idx_ref

	// compute convertion functions
	// count lenght of bins
	int len = 0; // length of combined bins
	idx_ref = 256; idx_mch = 256;
	while (idx_ref != 0 && idx_mch != 0) {
		// one more bin
		len ++;
		// move to next node
		const int last_ref = nodes[idx_ref][idx_mch].last_ref;
		const int last_mch = nodes[idx_ref][idx_mch].last_mch;;
		idx_ref = last_ref;  idx_mch = last_mch;
	} // while

	// fill conv_ref/mch
	int idx_bin = len - 1;
	idx_ref = 256; idx_mch = 256;

	while ((idx_ref != 0 && idx_mch != 0)) {
		const BYTE c = idx_bin * 256 / len;
		const int last_ref = nodes[idx_ref][idx_mch].last_ref;
		const int last_mch = nodes[idx_ref][idx_mch].last_mch;

		int i;
		for (i = last_ref; i < idx_ref; i ++)
			conv_ref[i] = c;
		for (i = last_mch; i < idx_mch; i ++)
			conv_mch[i] = c;

		// move to next node
		idx_ref = last_ref;  idx_mch = last_mch;
		idx_bin --;
	} // while
}

static void d_convert(BYTE* map, int len, const BYTE* conv) {
	BYTE* pmap = map.GetBufferT();
	BYTE* pmap_end = pmap + len;

	for (; pmap != pmap_end; pmap ++)
		*pmap = conv[*pmap];
}

static void d_imhist(BYTE* map, int len, int hist) {
	memset(hist, 0, 256 * sizeof(int));
	int i;
	for (i = 0; i < len; i ++)
		hist[map[i]] ++;
}

void dhw(BYTE* map_ref, BYTE* map_mch, int len) {
	int hist_ref[256];
	int hist_mch[256];

	// compute histograms
	d_imhist(map_ref, len, hist_ref);
	d_imhist(map_mch, len, hist_mch);

	BYTE conv_ref[256];
	BYTE conv_mch[256];
	// compute convert-tables
	d_match_hist(hist_ref, hist_mch, conv_ref, conv_mch);

	// convert
	d_convert(map_ref, len, conv_ref);
	d_convert(map_mch, len, conv_mch);
}
