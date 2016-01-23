#ifndef _DB_TP_H
#define _DB_TP_H
#include <sqlite3.h>
#include <stdint.h>

typedef enum
{
	polarization_vertical,
	polarization_horizontal,
	polarization_left,
	polarization_right,
	polarization_auto	/* using at lnbinfo_t */
} polarization_e;

typedef enum
{
	qam_none=-1,
	qam_4,
	qam_16,
	qam_32,
	qam_64,
	qam_128,
	qam_256,
} qam_e;

typedef enum
{
	mod_qpsk,	// QPSK
	mod_8psk,	// 8PSK
} modulation_e;

typedef enum
{
	fec_auto,
	fec_1_2,
	fec_2_3,
	fec_3_4,
	fec_3_5,
	fec_4_5,
	fec_5_6,
	fec_6_7,
	fec_7_8,
	fec_8_9,
	fec_9_10,
} fec_e;

typedef enum
{
	trans_dvb_s,
	trans_dvb_s2,
	trans_dvb_c,
	trans_dvb_t,
} transmission_e;

typedef enum
{
	terr_band_5MHz=5,
	terr_band_6MHz,
	terr_band_7MHz,
	terr_band_8MHz,
} terrestrial_band_e;

typedef enum
{
	pirot_off,	// dvb-s2 에만 해당.
	pirot_on,	
	pirot_unknown,	// dvb-s 에는 pirot 이 없다.
} pirot_e;

typedef struct t_tpInfo
{
	uint16_t	id;			/* TP ID : 1 ~ 65535 */
	uint16_t	sat_id;			/* TP가 포함된 위성 ID : 0 ~ 255 */
	unsigned int	frequency;		/* TP의 주파수 || sat 에선 Mhz, cable, terrestrial에선 kHz */
	unsigned int	symbol_rate;		/* TP의 심볼레이트 (Ksps) || 지상파에서는 BandWidth */
	polarization_e	polarization;		/* TP의 극성 */
	uint8_t		frequency_drift;	/* Frequency Drift || 지상파에서는 channel ID */
	qam_e		qam;			/* QAM 종류 */
	uint16_t	network_id;		/* Network ID */
	uint16_t	org_network_id;		/* Original Network ID */
	uint16_t	ts_id;			/* Transfer Stream ID */
	modulation_e	modulation;	
	fec_e		fec;
	transmission_e	transmission;
	char 		channel_number[5];	/* 지상파 channel number */
	pirot_e		pirot;			/* only using for dvb-s2 */
}tpinfo_t;

/*
 * \brief print tpinfo
 */
extern void db_tp_print( tpinfo_t *info );


/*
 * \brief select id from tpfino where sat_id = %d
 *
 * the selected id is stored at tp_id( dynamic mem allocated ), so you should call db_tp_id_free for free memory.
 */
extern int db_tp_select_id_by_sat_id( sqlite3 *db, uint16_t sat_id, uint16_t **tp_id );
/*
 * \brief select frequency from tpfino where id = %d
 */
extern int db_tp_select_frequency_by_id(sqlite3 *db,uint16_t id);

/*
 * \brief free memory which allocated from db_tp_select_id_by_sat_id
 */
extern void db_tp_id_free( uint16_t *tp_id );

/*
 * \brief select id from tpinfo where sat_id = %d  AND frequency = %d AND polarization = %d ",
 *
 * \ret 0 not found
 * \ret >0 found
 */
extern int db_tp_select_id_by_sat_id_freq_pol( sqlite3 *db, uint16_t sat_id, unsigned int frequency, polarization_e polarization );

/*
 * \brief select id from tpinfo where sat_id = %d  AND frequency = %d AND polarization = %d AND symbol_rate = %d",
 *
 * \ret 0 not found
 * \ret >0 found
 */
extern int db_tp_select_id_by_sat_id_freq_sym_pol( sqlite3 *db, uint16_t sat_id, 
		unsigned int frequency, unsigned int symbol_rate, polarization_e polarization );

/*
 * \brief "SELECT id FROM tpinfo WHERE sat_id = %d  AND frequency = %d AND symbol_rate = %d",
 *
 * \ret 0 not found
 * \ret >0 found
 */
extern int db_tp_select_id_by_sat_id_freq_sym( sqlite3 *db, uint16_t sat_id, unsigned int frequency, unsigned int symbol_rate );

/*
 * \brief "SELECT id FROM tpinfo WHERE sat_id = %d  AND frequency = %d AND symbol_rate = %d and qam = %d"
 *
 * \ret 0 not found
 * \ret >0 found
 */
extern int db_tp_select_id_by_sat_id_freq_sym_qam( sqlite3 *db, uint16_t sat_id, unsigned int frequency, unsigned int symbol_rate, qam_e qam );

/*
 * \ret 0 not found
 * \ret >0 found
 */
extern int db_tp_select_id_by_tpinfo(
		sqlite3 *db,
		tpinfo_t *tpinfo);

extern int db_tp_select_id_by_sat_id_and_frequency( sqlite3 *db, int sat_id, int frequency );
/*
 * \brief insert tpinfo.
 *
 * ret !0 inserted tpinfo`s id
 */
extern int db_tp_insert_tpinfo( sqlite3 *db, tpinfo_t *tpinfo );

/*
 * \brief DELETE FROM tpinfo WHERE id = %d
 */
extern int db_tp_delete_info_by_id(sqlite3 *db, uint16_t id);
/*
 * \brief DELETE FROM tpinfo WHERE sat_id = %d
 */
extern int db_tp_delete_info_by_sat_id(sqlite3 *db, uint16_t sat_id);


/*
 * \brief SELECT * FROM tpinfo WHERE id = %d
 */
extern int db_tp_select_info_by_id( sqlite3 *db, uint16_t id, tpinfo_t *info );

/*
 * \brief "UPDATE tpfino SET ts_id = %d, network_id = %d, org_network_id = %d WHERE id = %d"
 */
extern int db_tp_update_si_info( sqlite3 *db, uint16_t id, uint16_t ts_id, uint16_t network_id, 
		uint16_t org_network_id );

/*
 * \brief update pirot
 */
extern int db_tp_update_pirot( sqlite3 *db, uint16_t id, uint8_t pirot );

/*
 * \brief update tpinfo where tp id is tp_id (func for TP Ch edit menu)
 */
extern int db_tp_update_from_tp_edit( sqlite3 *db, tpinfo_t *tp_item  );

/*
 * \brief UPDATE tpinfo SET qam = %d, symbol_rate = %d WHERE id = %d
 */
extern int db_tp_update_qam_symbolrate( sqlite3 *db, uint16_t id, qam_e qam, unsigned int symbol_rate );

/*
 * \brief SELECT * FROM tpinfo WHERE sat_id = %d
 */
extern int db_tp_select_info_by_sat_id(sqlite3 *db, int sat_id, tpinfo_t **tp_tbl);

/*
 * \brief SELECT * FROM tpinfo WHERE sat_id = %d
 */
extern int db_tp_get_tp_num_by_sat_id(sqlite3 *db, int sat_id);

/*
 * \brief SELECT id FROM tpinfo WHERE sat_id = %d  AND frequency = %d AND polarization = %d AND 
 *		symbol_rate = %d AND modulation =  %d AND transmission = %d
 */
extern int db_tp_select_id_by_sat_id_freq_sym_pol_modulation_transmission( sqlite3 *db, uint16_t sat_id, 
		unsigned int frequency, unsigned int symbol_rate, polarization_e polarization,
		modulation_e modulation, transmission_e transmission );

/*
 * \brief SELECT id FROM tpinfo WHERE sat_id = %d  AND frequency = %d AND polarization = %d AND 
 *		symbol_rate = %d AND modulation =  %d AND fec = %d AND transmission = %d
 */
extern int db_tp_select_id_by_sat_id_freq_sym_pol_modulation_fec_transmission( sqlite3 *db, 
		uint16_t sat_id, unsigned int frequency, unsigned int symbol_rate, 
		polarization_e polarization, modulation_e modulation, fec_e fec, 
		transmission_e transmission );

/*
 * \brief if tp is satelite tp, return true.
 */
extern int db_tp_satellite( tpinfo_t *tpinfo );

/*
 * \brief network search할때 새로찾은 위성 (S or S2)TP가 db에 있는지 없는지 확인하는 함수
 *
 *	  SELECT id FROM tpinfo WHERE \
 *	  sat_id = %d  AND polarization = %d AND symbol_rate = %d AND modulation =%d \
 *	  polarization = %d AND transmission = %d AND frequency >= %d AND frequency <=  %d"
 */
extern int db_tp_sat_network_duplicate( sqlite3 *db, uint16_t sat_id, uint32_t frequency, uint32_t symbol_rate,
		polarization_e polarization, transmission_e transmission, modulation_e modulation );

/*
 * \brief sat_id, freq를 이용해 tpinfo을 얻어온다. return true
 */
extern int db_tp_select_info_by_sat_id_frequency( sqlite3 *db, uint16_t sat_id, uint32_t freq, tpinfo_t *info );
#ifdef BLIND_SCAN
extern int db_tp_update_symbol_trans_modul_pirot( sqlite3 *db, tpinfo_t *tp );
#endif

#endif

