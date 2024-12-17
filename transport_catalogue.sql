--
-- PostgreSQL database dump
--

-- Dumped from database version 17.2
-- Dumped by pg_dump version 17.2

-- Started on 2024-12-17 10:49:39

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- TOC entry 221 (class 1259 OID 16459)
-- Name: bus_stops; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.bus_stops (
    bus_id integer NOT NULL,
    stop_id integer,
    stop_position integer NOT NULL
);


ALTER TABLE public.bus_stops OWNER TO postgres;

--
-- TOC entry 220 (class 1259 OID 16404)
-- Name: buses; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.buses (
    id integer NOT NULL,
    name text NOT NULL,
    is_roundtrip boolean NOT NULL,
    color_index integer,
    bus_type text,
    capacity integer,
    has_wifi boolean,
    has_sockets boolean,
    is_night boolean,
    is_available boolean,
    price integer,
    is_day boolean DEFAULT false
);


ALTER TABLE public.buses OWNER TO postgres;

--
-- TOC entry 219 (class 1259 OID 16403)
-- Name: buses_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.buses_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.buses_id_seq OWNER TO postgres;

--
-- TOC entry 4913 (class 0 OID 0)
-- Dependencies: 219
-- Name: buses_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.buses_id_seq OWNED BY public.buses.id;


--
-- TOC entry 225 (class 1259 OID 16524)
-- Name: color_palette; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.color_palette (
    id integer NOT NULL,
    render_setting_id integer,
    color integer[]
);


ALTER TABLE public.color_palette OWNER TO postgres;

--
-- TOC entry 224 (class 1259 OID 16523)
-- Name: color_palette_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.color_palette_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.color_palette_id_seq OWNER TO postgres;

--
-- TOC entry 4914 (class 0 OID 0)
-- Dependencies: 224
-- Name: color_palette_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.color_palette_id_seq OWNED BY public.color_palette.id;


--
-- TOC entry 228 (class 1259 OID 16543)
-- Name: distances; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.distances (
    id integer NOT NULL,
    from_stop_id integer NOT NULL,
    to_stop_id integer NOT NULL,
    distance integer NOT NULL
);


ALTER TABLE public.distances OWNER TO postgres;

--
-- TOC entry 227 (class 1259 OID 16542)
-- Name: distances_new_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.distances_new_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.distances_new_id_seq OWNER TO postgres;

--
-- TOC entry 4915 (class 0 OID 0)
-- Dependencies: 227
-- Name: distances_new_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.distances_new_id_seq OWNED BY public.distances.id;


--
-- TOC entry 223 (class 1259 OID 16515)
-- Name: render_settings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.render_settings (
    id integer NOT NULL,
    bus_label_font_size integer,
    bus_label_offset integer[],
    stop_label_font_size integer,
    stop_label_offset integer[],
    stop_radius integer,
    underlayer_color numeric[],
    underlayer_width integer,
    line_width integer,
    padding integer,
    height integer,
    width integer
);


ALTER TABLE public.render_settings OWNER TO postgres;

--
-- TOC entry 222 (class 1259 OID 16514)
-- Name: render_settings_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.render_settings_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.render_settings_id_seq OWNER TO postgres;

--
-- TOC entry 4916 (class 0 OID 0)
-- Dependencies: 222
-- Name: render_settings_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.render_settings_id_seq OWNED BY public.render_settings.id;


--
-- TOC entry 226 (class 1259 OID 16537)
-- Name: routing_settings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.routing_settings (
    bus_velocity double precision DEFAULT 0.0 NOT NULL,
    bus_wait_time integer DEFAULT 0 NOT NULL
);


ALTER TABLE public.routing_settings OWNER TO postgres;

--
-- TOC entry 4917 (class 0 OID 0)
-- Dependencies: 226
-- Name: TABLE routing_settings; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE public.routing_settings IS 'Содержит настройки маршрутизации, такие как скорость автобуса и время ожидания на остановках.';


--
-- TOC entry 218 (class 1259 OID 16393)
-- Name: stops; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.stops (
    id integer NOT NULL,
    name text NOT NULL,
    latitude double precision NOT NULL,
    longitude double precision NOT NULL
);


ALTER TABLE public.stops OWNER TO postgres;

--
-- TOC entry 217 (class 1259 OID 16392)
-- Name: stops_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.stops_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.stops_id_seq OWNER TO postgres;

--
-- TOC entry 4918 (class 0 OID 0)
-- Dependencies: 217
-- Name: stops_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.stops_id_seq OWNED BY public.stops.id;


--
-- TOC entry 4724 (class 2604 OID 16407)
-- Name: buses id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.buses ALTER COLUMN id SET DEFAULT nextval('public.buses_id_seq'::regclass);


--
-- TOC entry 4727 (class 2604 OID 16527)
-- Name: color_palette id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.color_palette ALTER COLUMN id SET DEFAULT nextval('public.color_palette_id_seq'::regclass);


--
-- TOC entry 4730 (class 2604 OID 16546)
-- Name: distances id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.distances ALTER COLUMN id SET DEFAULT nextval('public.distances_new_id_seq'::regclass);


--
-- TOC entry 4726 (class 2604 OID 16518)
-- Name: render_settings id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.render_settings ALTER COLUMN id SET DEFAULT nextval('public.render_settings_id_seq'::regclass);


--
-- TOC entry 4723 (class 2604 OID 16396)
-- Name: stops id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.stops ALTER COLUMN id SET DEFAULT nextval('public.stops_id_seq'::regclass);


--
-- TOC entry 4900 (class 0 OID 16459)
-- Dependencies: 221
-- Data for Name: bus_stops; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.bus_stops VALUES (4, 3, 1);
INSERT INTO public.bus_stops VALUES (4, 6, 2);
INSERT INTO public.bus_stops VALUES (4, 7, 3);
INSERT INTO public.bus_stops VALUES (4, 28, 4);
INSERT INTO public.bus_stops VALUES (13, 17, 1);
INSERT INTO public.bus_stops VALUES (13, 18, 2);
INSERT INTO public.bus_stops VALUES (13, 20, 3);
INSERT INTO public.bus_stops VALUES (13, 19, 4);
INSERT INTO public.bus_stops VALUES (13, 7, 5);
INSERT INTO public.bus_stops VALUES (12, 24, 1);
INSERT INTO public.bus_stops VALUES (12, 22, 2);
INSERT INTO public.bus_stops VALUES (12, 21, 3);
INSERT INTO public.bus_stops VALUES (5, 9, 0);
INSERT INTO public.bus_stops VALUES (5, 10, 1);
INSERT INTO public.bus_stops VALUES (5, 28, 2);
INSERT INTO public.bus_stops VALUES (5, 8, 3);
INSERT INTO public.bus_stops VALUES (11, 13, 1);
INSERT INTO public.bus_stops VALUES (10, 20, 1);
INSERT INTO public.bus_stops VALUES (10, 21, 2);
INSERT INTO public.bus_stops VALUES (10, 22, 3);
INSERT INTO public.bus_stops VALUES (10, 23, 4);
INSERT INTO public.bus_stops VALUES (2, 2, 1);
INSERT INTO public.bus_stops VALUES (2, 28, 2);
INSERT INTO public.bus_stops VALUES (2, 3, 3);
INSERT INTO public.bus_stops VALUES (2, 4, 4);
INSERT INTO public.bus_stops VALUES (11, 16, 2);
INSERT INTO public.bus_stops VALUES (11, 24, 3);
INSERT INTO public.bus_stops VALUES (11, 19, 4);
INSERT INTO public.bus_stops VALUES (8, 25, 1);
INSERT INTO public.bus_stops VALUES (8, 26, 2);
INSERT INTO public.bus_stops VALUES (8, 27, 3);
INSERT INTO public.bus_stops VALUES (1, 2, 0);
INSERT INTO public.bus_stops VALUES (1, 1, 1);
INSERT INTO public.bus_stops VALUES (1, 4, 2);
INSERT INTO public.bus_stops VALUES (1, 7, 3);
INSERT INTO public.bus_stops VALUES (7, 16, 0);
INSERT INTO public.bus_stops VALUES (7, 17, 1);
INSERT INTO public.bus_stops VALUES (6, 12, 2);
INSERT INTO public.bus_stops VALUES (6, 13, 3);
INSERT INTO public.bus_stops VALUES (7, 9, 2);
INSERT INTO public.bus_stops VALUES (7, 8, 3);
INSERT INTO public.bus_stops VALUES (7, 16, 4);
INSERT INTO public.bus_stops VALUES (6, 14, 4);
INSERT INTO public.bus_stops VALUES (6, 15, 5);
INSERT INTO public.bus_stops VALUES (6, 1, 6);
INSERT INTO public.bus_stops VALUES (6, 28, 7);
INSERT INTO public.bus_stops VALUES (3, 5, 1);
INSERT INTO public.bus_stops VALUES (3, 28, 2);
INSERT INTO public.bus_stops VALUES (3, 4, 3);
INSERT INTO public.bus_stops VALUES (3, 6, 4);
INSERT INTO public.bus_stops VALUES (3, 5, 5);


--
-- TOC entry 4899 (class 0 OID 16404)
-- Dependencies: 220
-- Data for Name: buses; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.buses VALUES (13, '508', true, 6, 'trolleybus', 45, false, true, true, true, 65, true);
INSERT INTO public.buses VALUES (8, '400', false, 7, 'electrobus', 80, true, true, false, true, 75, false);
INSERT INTO public.buses VALUES (6, '55', false, 8, 'autobus', 70, false, false, false, true, 65, false);
INSERT INTO public.buses VALUES (2, '635', false, 9, 'electrobus', 40, true, true, true, true, 70, false);
INSERT INTO public.buses VALUES (12, '204', false, 10, 'electrobus', 50, true, true, false, true, 50, false);
INSERT INTO public.buses VALUES (11, '812', false, 11, 'autobus', 60, true, false, false, true, 70, false);
INSERT INTO public.buses VALUES (7, '114', true, 12, 'trolleybus', 65, false, true, false, true, 45, true);
INSERT INTO public.buses VALUES (10, '306', false, 2, 'trolleybus', 50, false, true, false, true, 55, false);
INSERT INTO public.buses VALUES (3, '750', true, 3, 'trolleybus', 60, false, true, false, true, 60, false);
INSERT INTO public.buses VALUES (4, '850', false, 4, 'autobus', 45, false, false, true, true, 55, false);
INSERT INTO public.buses VALUES (5, '123', false, 5, 'electrobus', 30, true, true, false, true, 40, false);
INSERT INTO public.buses VALUES (1, '297', true, 1, 'autobus', 50, false, false, false, true, 50, false);


--
-- TOC entry 4904 (class 0 OID 16524)
-- Dependencies: 225
-- Data for Name: color_palette; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.color_palette VALUES (1, 1, '{237,237,237}');
INSERT INTO public.color_palette VALUES (2, 1, '{255,197,251}');
INSERT INTO public.color_palette VALUES (3, 1, '{214,197,247}');
INSERT INTO public.color_palette VALUES (4, 1, '{198,247,197}');
INSERT INTO public.color_palette VALUES (5, 1, '{247,238,197}');
INSERT INTO public.color_palette VALUES (6, 1, '{158,182,255}');
INSERT INTO public.color_palette VALUES (7, 1, '{224,255,255}');
INSERT INTO public.color_palette VALUES (8, 1, '{255,46,46}');
INSERT INTO public.color_palette VALUES (9, 1, '{175,255,195}');
INSERT INTO public.color_palette VALUES (10, 1, '{250,235,215}');
INSERT INTO public.color_palette VALUES (11, 1, '{240,230,140}');
INSERT INTO public.color_palette VALUES (12, 1, '{255,228,225}');
INSERT INTO public.color_palette VALUES (13, 1, '{245,222,179}');
INSERT INTO public.color_palette VALUES (14, 1, '{250,250,210}');
INSERT INTO public.color_palette VALUES (15, 1, '{221,160,221}');
INSERT INTO public.color_palette VALUES (16, 1, '{255,182,193}');
INSERT INTO public.color_palette VALUES (17, 1, '{255,240,245}');
INSERT INTO public.color_palette VALUES (18, 1, '{230,230,250}');
INSERT INTO public.color_palette VALUES (19, 1, '{255,239,213}');
INSERT INTO public.color_palette VALUES (20, 1, '{255,250,205}');
INSERT INTO public.color_palette VALUES (21, 1, '{245,245,220}');
INSERT INTO public.color_palette VALUES (22, 1, '{240,248,255}');
INSERT INTO public.color_palette VALUES (23, 1, '{255,235,205}');
INSERT INTO public.color_palette VALUES (24, 1, '{240,255,240}');
INSERT INTO public.color_palette VALUES (25, 1, '{245,245,245}');
INSERT INTO public.color_palette VALUES (26, 1, '{255,248,220}');
INSERT INTO public.color_palette VALUES (27, 1, '{248,248,255}');
INSERT INTO public.color_palette VALUES (28, 1, '{255,228,196}');


--
-- TOC entry 4907 (class 0 OID 16543)
-- Dependencies: 228
-- Data for Name: distances; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.distances VALUES (1, 1, 2, 2504);
INSERT INTO public.distances VALUES (2, 1, 3, 2810);
INSERT INTO public.distances VALUES (3, 1, 4, 1083);
INSERT INTO public.distances VALUES (4, 1, 5, 2074);
INSERT INTO public.distances VALUES (5, 1, 6, 2580);
INSERT INTO public.distances VALUES (6, 1, 7, 2059);
INSERT INTO public.distances VALUES (7, 1, 8, 2065);
INSERT INTO public.distances VALUES (8, 1, 9, 1597);
INSERT INTO public.distances VALUES (9, 1, 10, 2330);
INSERT INTO public.distances VALUES (10, 1, 11, 2770);
INSERT INTO public.distances VALUES (11, 1, 12, 2458);
INSERT INTO public.distances VALUES (12, 1, 13, 1270);
INSERT INTO public.distances VALUES (13, 1, 14, 2385);
INSERT INTO public.distances VALUES (14, 1, 15, 1750);
INSERT INTO public.distances VALUES (15, 1, 16, 2383);
INSERT INTO public.distances VALUES (16, 1, 17, 1623);
INSERT INTO public.distances VALUES (17, 1, 18, 1155);
INSERT INTO public.distances VALUES (18, 1, 19, 968);
INSERT INTO public.distances VALUES (19, 1, 20, 2310);
INSERT INTO public.distances VALUES (20, 1, 21, 1493);
INSERT INTO public.distances VALUES (21, 1, 22, 2486);
INSERT INTO public.distances VALUES (22, 1, 23, 2009);
INSERT INTO public.distances VALUES (23, 1, 24, 1864);
INSERT INTO public.distances VALUES (24, 1, 25, 1361);
INSERT INTO public.distances VALUES (25, 1, 26, 2449);
INSERT INTO public.distances VALUES (26, 1, 27, 2415);
INSERT INTO public.distances VALUES (27, 1, 28, 1236);
INSERT INTO public.distances VALUES (28, 2, 3, 2198);
INSERT INTO public.distances VALUES (29, 2, 4, 2678);
INSERT INTO public.distances VALUES (30, 2, 5, 1449);
INSERT INTO public.distances VALUES (31, 2, 6, 1649);
INSERT INTO public.distances VALUES (32, 2, 7, 2947);
INSERT INTO public.distances VALUES (33, 2, 8, 975);
INSERT INTO public.distances VALUES (34, 2, 9, 1073);
INSERT INTO public.distances VALUES (35, 2, 10, 2626);
INSERT INTO public.distances VALUES (36, 2, 11, 2875);
INSERT INTO public.distances VALUES (37, 2, 12, 2979);
INSERT INTO public.distances VALUES (38, 2, 13, 2457);
INSERT INTO public.distances VALUES (39, 2, 14, 1544);
INSERT INTO public.distances VALUES (40, 2, 15, 1434);
INSERT INTO public.distances VALUES (41, 2, 16, 1852);
INSERT INTO public.distances VALUES (42, 2, 17, 2024);
INSERT INTO public.distances VALUES (43, 2, 18, 2232);
INSERT INTO public.distances VALUES (44, 2, 19, 1745);
INSERT INTO public.distances VALUES (45, 2, 20, 2308);
INSERT INTO public.distances VALUES (46, 2, 21, 2733);
INSERT INTO public.distances VALUES (47, 2, 22, 1268);
INSERT INTO public.distances VALUES (48, 2, 23, 2356);
INSERT INTO public.distances VALUES (49, 2, 24, 1011);
INSERT INTO public.distances VALUES (50, 2, 25, 1581);
INSERT INTO public.distances VALUES (51, 2, 26, 2482);
INSERT INTO public.distances VALUES (52, 2, 27, 2484);
INSERT INTO public.distances VALUES (53, 2, 28, 2814);
INSERT INTO public.distances VALUES (54, 3, 4, 1532);
INSERT INTO public.distances VALUES (55, 3, 5, 1826);
INSERT INTO public.distances VALUES (56, 3, 6, 2031);
INSERT INTO public.distances VALUES (57, 3, 7, 2077);
INSERT INTO public.distances VALUES (58, 3, 8, 1147);
INSERT INTO public.distances VALUES (59, 3, 9, 2688);
INSERT INTO public.distances VALUES (60, 3, 10, 2101);
INSERT INTO public.distances VALUES (61, 3, 11, 2450);
INSERT INTO public.distances VALUES (62, 3, 12, 1846);
INSERT INTO public.distances VALUES (63, 3, 13, 1777);
INSERT INTO public.distances VALUES (64, 3, 14, 2382);
INSERT INTO public.distances VALUES (65, 3, 15, 2064);
INSERT INTO public.distances VALUES (66, 3, 16, 2727);
INSERT INTO public.distances VALUES (67, 3, 17, 925);
INSERT INTO public.distances VALUES (68, 3, 18, 2241);
INSERT INTO public.distances VALUES (69, 3, 19, 1459);
INSERT INTO public.distances VALUES (70, 3, 20, 1278);
INSERT INTO public.distances VALUES (71, 3, 21, 2315);
INSERT INTO public.distances VALUES (72, 3, 22, 2515);
INSERT INTO public.distances VALUES (73, 3, 23, 1675);
INSERT INTO public.distances VALUES (74, 3, 24, 2134);
INSERT INTO public.distances VALUES (75, 3, 25, 1623);
INSERT INTO public.distances VALUES (76, 3, 26, 2588);
INSERT INTO public.distances VALUES (77, 3, 27, 1902);
INSERT INTO public.distances VALUES (78, 3, 28, 2404);
INSERT INTO public.distances VALUES (79, 4, 5, 1419);
INSERT INTO public.distances VALUES (80, 4, 6, 2314);
INSERT INTO public.distances VALUES (81, 4, 7, 2610);
INSERT INTO public.distances VALUES (82, 4, 8, 2785);
INSERT INTO public.distances VALUES (83, 4, 9, 2747);
INSERT INTO public.distances VALUES (84, 4, 10, 2008);
INSERT INTO public.distances VALUES (85, 4, 11, 1241);
INSERT INTO public.distances VALUES (86, 4, 12, 1954);
INSERT INTO public.distances VALUES (87, 4, 13, 1848);
INSERT INTO public.distances VALUES (88, 4, 14, 2312);
INSERT INTO public.distances VALUES (89, 4, 15, 1414);
INSERT INTO public.distances VALUES (90, 4, 16, 1316);
INSERT INTO public.distances VALUES (91, 4, 17, 1538);
INSERT INTO public.distances VALUES (92, 4, 18, 1342);
INSERT INTO public.distances VALUES (93, 4, 19, 2173);
INSERT INTO public.distances VALUES (94, 4, 20, 2988);
INSERT INTO public.distances VALUES (95, 4, 21, 953);
INSERT INTO public.distances VALUES (96, 4, 22, 1470);
INSERT INTO public.distances VALUES (97, 4, 23, 1357);
INSERT INTO public.distances VALUES (98, 4, 24, 2345);
INSERT INTO public.distances VALUES (99, 4, 25, 2537);
INSERT INTO public.distances VALUES (100, 4, 26, 1357);
INSERT INTO public.distances VALUES (101, 4, 27, 2830);
INSERT INTO public.distances VALUES (102, 4, 28, 968);
INSERT INTO public.distances VALUES (103, 5, 6, 2411);
INSERT INTO public.distances VALUES (104, 5, 7, 1753);
INSERT INTO public.distances VALUES (105, 5, 8, 1735);
INSERT INTO public.distances VALUES (106, 5, 9, 2032);
INSERT INTO public.distances VALUES (107, 5, 10, 2703);
INSERT INTO public.distances VALUES (108, 5, 11, 1391);
INSERT INTO public.distances VALUES (109, 5, 12, 2592);
INSERT INTO public.distances VALUES (110, 5, 13, 1572);
INSERT INTO public.distances VALUES (111, 5, 14, 993);
INSERT INTO public.distances VALUES (112, 5, 15, 1695);
INSERT INTO public.distances VALUES (113, 5, 16, 2870);
INSERT INTO public.distances VALUES (114, 5, 17, 1603);
INSERT INTO public.distances VALUES (115, 5, 18, 2131);
INSERT INTO public.distances VALUES (116, 5, 19, 1942);
INSERT INTO public.distances VALUES (117, 5, 20, 914);
INSERT INTO public.distances VALUES (118, 5, 21, 2578);
INSERT INTO public.distances VALUES (119, 5, 22, 2622);
INSERT INTO public.distances VALUES (120, 5, 23, 2791);
INSERT INTO public.distances VALUES (121, 5, 24, 2341);
INSERT INTO public.distances VALUES (122, 5, 25, 1127);
INSERT INTO public.distances VALUES (123, 5, 26, 2233);
INSERT INTO public.distances VALUES (124, 5, 27, 1279);
INSERT INTO public.distances VALUES (125, 5, 28, 1333);
INSERT INTO public.distances VALUES (126, 6, 7, 1809);
INSERT INTO public.distances VALUES (127, 6, 8, 1775);
INSERT INTO public.distances VALUES (128, 6, 9, 2735);
INSERT INTO public.distances VALUES (129, 6, 10, 2879);
INSERT INTO public.distances VALUES (130, 6, 11, 1455);
INSERT INTO public.distances VALUES (131, 6, 12, 2737);
INSERT INTO public.distances VALUES (132, 6, 13, 1129);
INSERT INTO public.distances VALUES (133, 6, 14, 2888);
INSERT INTO public.distances VALUES (134, 6, 15, 2359);
INSERT INTO public.distances VALUES (135, 6, 16, 984);
INSERT INTO public.distances VALUES (136, 6, 17, 976);
INSERT INTO public.distances VALUES (137, 6, 18, 2527);
INSERT INTO public.distances VALUES (138, 6, 19, 1519);
INSERT INTO public.distances VALUES (139, 6, 20, 1707);
INSERT INTO public.distances VALUES (140, 6, 21, 1534);
INSERT INTO public.distances VALUES (141, 6, 22, 2369);
INSERT INTO public.distances VALUES (142, 6, 23, 1993);
INSERT INTO public.distances VALUES (143, 6, 24, 2498);
INSERT INTO public.distances VALUES (144, 6, 25, 1189);
INSERT INTO public.distances VALUES (145, 6, 26, 1635);
INSERT INTO public.distances VALUES (146, 6, 27, 2023);
INSERT INTO public.distances VALUES (147, 6, 28, 2068);
INSERT INTO public.distances VALUES (148, 7, 8, 2072);
INSERT INTO public.distances VALUES (149, 7, 9, 2533);
INSERT INTO public.distances VALUES (150, 7, 10, 1740);
INSERT INTO public.distances VALUES (151, 7, 11, 1669);
INSERT INTO public.distances VALUES (152, 7, 12, 1702);
INSERT INTO public.distances VALUES (153, 7, 13, 1372);
INSERT INTO public.distances VALUES (154, 7, 14, 2881);
INSERT INTO public.distances VALUES (155, 7, 15, 1610);
INSERT INTO public.distances VALUES (156, 7, 16, 2306);
INSERT INTO public.distances VALUES (157, 7, 17, 2765);
INSERT INTO public.distances VALUES (158, 7, 18, 2595);
INSERT INTO public.distances VALUES (159, 7, 19, 2507);
INSERT INTO public.distances VALUES (160, 7, 20, 1890);
INSERT INTO public.distances VALUES (161, 7, 21, 982);
INSERT INTO public.distances VALUES (162, 7, 22, 1843);
INSERT INTO public.distances VALUES (163, 7, 23, 2124);
INSERT INTO public.distances VALUES (164, 7, 24, 2065);
INSERT INTO public.distances VALUES (165, 7, 25, 1818);
INSERT INTO public.distances VALUES (166, 7, 26, 951);
INSERT INTO public.distances VALUES (167, 7, 27, 1918);
INSERT INTO public.distances VALUES (168, 7, 28, 2395);
INSERT INTO public.distances VALUES (169, 8, 9, 1413);
INSERT INTO public.distances VALUES (170, 8, 10, 1635);
INSERT INTO public.distances VALUES (171, 8, 11, 1385);
INSERT INTO public.distances VALUES (172, 8, 12, 2827);
INSERT INTO public.distances VALUES (173, 8, 13, 2967);
INSERT INTO public.distances VALUES (174, 8, 14, 902);
INSERT INTO public.distances VALUES (175, 8, 15, 2332);
INSERT INTO public.distances VALUES (176, 8, 16, 1438);
INSERT INTO public.distances VALUES (177, 8, 17, 2927);
INSERT INTO public.distances VALUES (178, 8, 18, 971);
INSERT INTO public.distances VALUES (179, 8, 19, 2655);
INSERT INTO public.distances VALUES (180, 8, 20, 1101);
INSERT INTO public.distances VALUES (181, 8, 21, 1520);
INSERT INTO public.distances VALUES (182, 8, 22, 2664);
INSERT INTO public.distances VALUES (183, 8, 23, 2998);
INSERT INTO public.distances VALUES (184, 8, 24, 1518);
INSERT INTO public.distances VALUES (185, 8, 25, 1686);
INSERT INTO public.distances VALUES (186, 8, 26, 1534);
INSERT INTO public.distances VALUES (187, 8, 27, 1610);
INSERT INTO public.distances VALUES (188, 8, 28, 2711);
INSERT INTO public.distances VALUES (189, 9, 10, 1510);
INSERT INTO public.distances VALUES (190, 9, 11, 1885);
INSERT INTO public.distances VALUES (191, 9, 12, 2333);
INSERT INTO public.distances VALUES (192, 9, 13, 1068);
INSERT INTO public.distances VALUES (193, 9, 14, 2360);
INSERT INTO public.distances VALUES (194, 9, 15, 2255);
INSERT INTO public.distances VALUES (195, 9, 16, 999);
INSERT INTO public.distances VALUES (196, 9, 17, 1532);
INSERT INTO public.distances VALUES (197, 9, 18, 2435);
INSERT INTO public.distances VALUES (198, 9, 19, 2529);
INSERT INTO public.distances VALUES (199, 9, 20, 1826);
INSERT INTO public.distances VALUES (200, 9, 21, 2707);
INSERT INTO public.distances VALUES (201, 9, 22, 1724);
INSERT INTO public.distances VALUES (202, 9, 23, 2328);
INSERT INTO public.distances VALUES (203, 9, 24, 2035);
INSERT INTO public.distances VALUES (204, 9, 25, 2628);
INSERT INTO public.distances VALUES (205, 9, 26, 1836);
INSERT INTO public.distances VALUES (206, 9, 27, 2773);
INSERT INTO public.distances VALUES (207, 9, 28, 2216);
INSERT INTO public.distances VALUES (208, 10, 11, 2216);
INSERT INTO public.distances VALUES (209, 10, 12, 1686);
INSERT INTO public.distances VALUES (210, 10, 13, 2727);
INSERT INTO public.distances VALUES (211, 10, 14, 1336);
INSERT INTO public.distances VALUES (212, 10, 15, 1245);
INSERT INTO public.distances VALUES (213, 10, 16, 1660);
INSERT INTO public.distances VALUES (214, 10, 17, 1416);
INSERT INTO public.distances VALUES (215, 10, 18, 2053);
INSERT INTO public.distances VALUES (216, 10, 19, 2361);
INSERT INTO public.distances VALUES (217, 10, 20, 1745);
INSERT INTO public.distances VALUES (218, 10, 21, 1425);
INSERT INTO public.distances VALUES (219, 10, 22, 2705);
INSERT INTO public.distances VALUES (220, 10, 23, 2788);
INSERT INTO public.distances VALUES (221, 10, 24, 1732);
INSERT INTO public.distances VALUES (222, 10, 25, 1274);
INSERT INTO public.distances VALUES (223, 10, 26, 2630);
INSERT INTO public.distances VALUES (224, 10, 27, 2534);
INSERT INTO public.distances VALUES (225, 10, 28, 2841);
INSERT INTO public.distances VALUES (243, 12, 13, 2045);
INSERT INTO public.distances VALUES (244, 12, 14, 1010);
INSERT INTO public.distances VALUES (245, 12, 15, 2205);
INSERT INTO public.distances VALUES (246, 12, 16, 1772);
INSERT INTO public.distances VALUES (247, 12, 17, 2346);
INSERT INTO public.distances VALUES (248, 12, 18, 2834);
INSERT INTO public.distances VALUES (249, 12, 19, 2244);
INSERT INTO public.distances VALUES (250, 12, 20, 1270);
INSERT INTO public.distances VALUES (251, 12, 21, 2671);
INSERT INTO public.distances VALUES (252, 12, 22, 1186);
INSERT INTO public.distances VALUES (253, 12, 23, 1480);
INSERT INTO public.distances VALUES (254, 12, 24, 1415);
INSERT INTO public.distances VALUES (255, 12, 25, 1343);
INSERT INTO public.distances VALUES (256, 12, 26, 2718);
INSERT INTO public.distances VALUES (257, 12, 27, 2128);
INSERT INTO public.distances VALUES (258, 12, 28, 2509);
INSERT INTO public.distances VALUES (259, 13, 14, 1877);
INSERT INTO public.distances VALUES (260, 13, 15, 1262);
INSERT INTO public.distances VALUES (261, 13, 16, 2002);
INSERT INTO public.distances VALUES (262, 13, 17, 1040);
INSERT INTO public.distances VALUES (263, 13, 18, 1563);
INSERT INTO public.distances VALUES (264, 13, 19, 1676);
INSERT INTO public.distances VALUES (265, 13, 20, 2960);
INSERT INTO public.distances VALUES (266, 13, 21, 1238);
INSERT INTO public.distances VALUES (267, 13, 22, 2429);
INSERT INTO public.distances VALUES (268, 13, 23, 2589);
INSERT INTO public.distances VALUES (269, 13, 24, 1402);
INSERT INTO public.distances VALUES (270, 13, 25, 2822);
INSERT INTO public.distances VALUES (271, 13, 26, 1900);
INSERT INTO public.distances VALUES (272, 13, 27, 2445);
INSERT INTO public.distances VALUES (273, 13, 28, 1345);
INSERT INTO public.distances VALUES (274, 14, 15, 2624);
INSERT INTO public.distances VALUES (275, 14, 16, 2405);
INSERT INTO public.distances VALUES (276, 14, 17, 1835);
INSERT INTO public.distances VALUES (277, 14, 18, 2577);
INSERT INTO public.distances VALUES (278, 14, 19, 1736);
INSERT INTO public.distances VALUES (279, 14, 20, 2367);
INSERT INTO public.distances VALUES (280, 14, 21, 2898);
INSERT INTO public.distances VALUES (281, 14, 22, 1365);
INSERT INTO public.distances VALUES (282, 14, 23, 2078);
INSERT INTO public.distances VALUES (283, 14, 24, 2869);
INSERT INTO public.distances VALUES (284, 14, 25, 1651);
INSERT INTO public.distances VALUES (285, 14, 26, 1929);
INSERT INTO public.distances VALUES (286, 14, 27, 2669);
INSERT INTO public.distances VALUES (287, 14, 28, 1026);
INSERT INTO public.distances VALUES (288, 15, 16, 2106);
INSERT INTO public.distances VALUES (289, 15, 17, 1033);
INSERT INTO public.distances VALUES (290, 15, 18, 2954);
INSERT INTO public.distances VALUES (291, 15, 19, 1130);
INSERT INTO public.distances VALUES (292, 15, 20, 1192);
INSERT INTO public.distances VALUES (293, 15, 21, 2509);
INSERT INTO public.distances VALUES (294, 15, 22, 1248);
INSERT INTO public.distances VALUES (295, 15, 23, 1665);
INSERT INTO public.distances VALUES (296, 15, 24, 2285);
INSERT INTO public.distances VALUES (297, 15, 25, 2973);
INSERT INTO public.distances VALUES (298, 15, 26, 1994);
INSERT INTO public.distances VALUES (299, 15, 27, 2778);
INSERT INTO public.distances VALUES (300, 15, 28, 2836);
INSERT INTO public.distances VALUES (301, 16, 17, 1606);
INSERT INTO public.distances VALUES (302, 16, 18, 2461);
INSERT INTO public.distances VALUES (303, 16, 19, 2713);
INSERT INTO public.distances VALUES (304, 16, 20, 974);
INSERT INTO public.distances VALUES (305, 16, 21, 2901);
INSERT INTO public.distances VALUES (306, 16, 22, 1808);
INSERT INTO public.distances VALUES (307, 16, 23, 2245);
INSERT INTO public.distances VALUES (308, 16, 24, 1761);
INSERT INTO public.distances VALUES (309, 16, 25, 1002);
INSERT INTO public.distances VALUES (310, 16, 26, 1990);
INSERT INTO public.distances VALUES (311, 16, 27, 1948);
INSERT INTO public.distances VALUES (312, 16, 28, 1107);
INSERT INTO public.distances VALUES (313, 17, 18, 2787);
INSERT INTO public.distances VALUES (314, 17, 19, 2047);
INSERT INTO public.distances VALUES (315, 17, 20, 1144);
INSERT INTO public.distances VALUES (316, 17, 21, 1103);
INSERT INTO public.distances VALUES (317, 17, 22, 2785);
INSERT INTO public.distances VALUES (318, 17, 23, 2107);
INSERT INTO public.distances VALUES (319, 17, 24, 1216);
INSERT INTO public.distances VALUES (320, 17, 25, 2857);
INSERT INTO public.distances VALUES (321, 17, 26, 1152);
INSERT INTO public.distances VALUES (322, 17, 27, 2313);
INSERT INTO public.distances VALUES (323, 17, 28, 2164);
INSERT INTO public.distances VALUES (324, 18, 19, 1921);
INSERT INTO public.distances VALUES (325, 18, 20, 1439);
INSERT INTO public.distances VALUES (326, 18, 21, 2760);
INSERT INTO public.distances VALUES (327, 18, 22, 1424);
INSERT INTO public.distances VALUES (328, 18, 23, 995);
INSERT INTO public.distances VALUES (329, 18, 24, 2910);
INSERT INTO public.distances VALUES (330, 18, 25, 2301);
INSERT INTO public.distances VALUES (331, 18, 26, 2689);
INSERT INTO public.distances VALUES (332, 18, 27, 1372);
INSERT INTO public.distances VALUES (333, 18, 28, 2862);
INSERT INTO public.distances VALUES (334, 19, 20, 1163);
INSERT INTO public.distances VALUES (335, 19, 21, 1545);
INSERT INTO public.distances VALUES (336, 19, 22, 1585);
INSERT INTO public.distances VALUES (337, 19, 23, 1286);
INSERT INTO public.distances VALUES (338, 19, 24, 1613);
INSERT INTO public.distances VALUES (339, 19, 25, 1766);
INSERT INTO public.distances VALUES (340, 19, 26, 2561);
INSERT INTO public.distances VALUES (341, 19, 27, 1197);
INSERT INTO public.distances VALUES (342, 19, 28, 1297);
INSERT INTO public.distances VALUES (343, 20, 21, 1511);
INSERT INTO public.distances VALUES (344, 20, 22, 1548);
INSERT INTO public.distances VALUES (345, 20, 23, 2297);
INSERT INTO public.distances VALUES (346, 20, 24, 1901);
INSERT INTO public.distances VALUES (347, 20, 25, 1127);
INSERT INTO public.distances VALUES (348, 20, 26, 1513);
INSERT INTO public.distances VALUES (349, 20, 27, 2112);
INSERT INTO public.distances VALUES (350, 20, 28, 931);
INSERT INTO public.distances VALUES (351, 21, 22, 1985);
INSERT INTO public.distances VALUES (352, 21, 23, 1766);
INSERT INTO public.distances VALUES (353, 21, 24, 1745);
INSERT INTO public.distances VALUES (354, 21, 25, 2457);
INSERT INTO public.distances VALUES (355, 21, 26, 2748);
INSERT INTO public.distances VALUES (356, 21, 27, 1850);
INSERT INTO public.distances VALUES (357, 21, 28, 2726);
INSERT INTO public.distances VALUES (358, 22, 23, 2581);
INSERT INTO public.distances VALUES (359, 22, 24, 2846);
INSERT INTO public.distances VALUES (360, 22, 25, 1990);
INSERT INTO public.distances VALUES (361, 22, 26, 2230);
INSERT INTO public.distances VALUES (362, 22, 27, 2629);
INSERT INTO public.distances VALUES (363, 22, 28, 1553);
INSERT INTO public.distances VALUES (364, 23, 24, 2394);
INSERT INTO public.distances VALUES (365, 23, 25, 967);
INSERT INTO public.distances VALUES (366, 23, 26, 1933);
INSERT INTO public.distances VALUES (367, 23, 27, 1100);
INSERT INTO public.distances VALUES (368, 23, 28, 2553);
INSERT INTO public.distances VALUES (369, 24, 25, 2220);
INSERT INTO public.distances VALUES (370, 24, 26, 1890);
INSERT INTO public.distances VALUES (371, 24, 27, 2834);
INSERT INTO public.distances VALUES (372, 24, 28, 1899);
INSERT INTO public.distances VALUES (373, 25, 26, 2973);
INSERT INTO public.distances VALUES (374, 25, 27, 1893);
INSERT INTO public.distances VALUES (375, 25, 28, 1841);
INSERT INTO public.distances VALUES (376, 26, 27, 1310);
INSERT INTO public.distances VALUES (377, 26, 28, 2736);
INSERT INTO public.distances VALUES (378, 27, 28, 1611);
INSERT INTO public.distances VALUES (405, 13, 29, 1424);
INSERT INTO public.distances VALUES (408, 13, 32, 1234);
INSERT INTO public.distances VALUES (409, 13, 33, 123);
INSERT INTO public.distances VALUES (414, 13, 36, 434);
INSERT INTO public.distances VALUES (417, 13, 38, 4231);
INSERT INTO public.distances VALUES (418, 13, 41, 33);
INSERT INTO public.distances VALUES (421, 13, 45, 423);
INSERT INTO public.distances VALUES (422, 1, 45, 1260);
INSERT INTO public.distances VALUES (423, 2, 45, 800);
INSERT INTO public.distances VALUES (427, 13, 39, 1232);
INSERT INTO public.distances VALUES (428, 13, 42, 1200);
INSERT INTO public.distances VALUES (429, 6, 42, 3000);
INSERT INTO public.distances VALUES (430, 7, 43, 2000);
INSERT INTO public.distances VALUES (431, 7, 44, 3000);
INSERT INTO public.distances VALUES (432, 5, 45, 444);
INSERT INTO public.distances VALUES (433, 13, 46, 444);
INSERT INTO public.distances VALUES (434, 13, 47, 555);
INSERT INTO public.distances VALUES (439, 13, 50, 44);
INSERT INTO public.distances VALUES (440, 5, 51, 555);
INSERT INTO public.distances VALUES (441, 5, 52, 55);
INSERT INTO public.distances VALUES (442, 5, 53, 444);
INSERT INTO public.distances VALUES (444, 5, 55, 55);
INSERT INTO public.distances VALUES (445, 5, 56, 555);
INSERT INTO public.distances VALUES (446, 5, 57, 555);
INSERT INTO public.distances VALUES (447, 5, 58, 123);
INSERT INTO public.distances VALUES (449, 5, 60, 555);
INSERT INTO public.distances VALUES (450, 5, 61, 555);
INSERT INTO public.distances VALUES (451, 5, 62, 555);
INSERT INTO public.distances VALUES (452, 5, 64, 1500);
INSERT INTO public.distances VALUES (453, 5, 65, 554);


--
-- TOC entry 4902 (class 0 OID 16515)
-- Dependencies: 223
-- Data for Name: render_settings; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.render_settings VALUES (1, 20, '{7,15}', 20, '{7,-3}', 5, '{255,255,255,0.85}', 3, 14, 30, 1000, 1000);


--
-- TOC entry 4905 (class 0 OID 16537)
-- Dependencies: 226
-- Data for Name: routing_settings; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.routing_settings VALUES (40, 6);


--
-- TOC entry 4897 (class 0 OID 16393)
-- Dependencies: 218
-- Data for Name: stops; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO public.stops VALUES (2, 'Бирюлёво Товарная', 55.5825, 37.68);
INSERT INTO public.stops VALUES (3, 'Универсам', 55.5845, 37.85);
INSERT INTO public.stops VALUES (1, 'Бирюлёво Западное', 55.6, 37.6517);
INSERT INTO public.stops VALUES (15, 'Рязанский Проспект', 55.83, 37.9);
INSERT INTO public.stops VALUES (14, 'Садовод', 55.815, 37.9);
INSERT INTO public.stops VALUES (13, 'МКАД', 55.8, 37.89);
INSERT INTO public.stops VALUES (12, 'Городской ТЦ', 55.78, 37.88);
INSERT INTO public.stops VALUES (4, 'Пражская', 55.62, 37.603124);
INSERT INTO public.stops VALUES (5, 'Марьино', 55.66, 37.733439);
INSERT INTO public.stops VALUES (6, 'Очаково', 55.68, 37.512817);
INSERT INTO public.stops VALUES (7, 'Кунцево', 55.729611, 37.54);
INSERT INTO public.stops VALUES (9, 'Розовый Сад', 55.76, 37.62);
INSERT INTO public.stops VALUES (10, 'Площадь Тюльпанов', 55.77, 37.6);
INSERT INTO public.stops VALUES (16, 'Северная Станция', 55.85, 37.8);
INSERT INTO public.stops VALUES (17, 'Зелёный Парк', 55.86, 37.59);
INSERT INTO public.stops VALUES (24, 'Набережная', 55.93, 37.64);
INSERT INTO public.stops VALUES (25, 'Центр Города', 55.94, 37.65);
INSERT INTO public.stops VALUES (28, 'Центральный Узел', 55.8, 37.7);
INSERT INTO public.stops VALUES (19, 'Горная Площадь', 55.8, 37.53);
INSERT INTO public.stops VALUES (20, 'Парк Водопадов', 55.89, 37.5);
INSERT INTO public.stops VALUES (18, 'Серебряный Парк', 55.87, 37.7);
INSERT INTO public.stops VALUES (23, 'Кленовая Улица', 55.92, 37.5);
INSERT INTO public.stops VALUES (22, 'Солнечный Холм', 55.95, 37.6);
INSERT INTO public.stops VALUES (27, 'Центральный Парк', 55.96, 37.9);
INSERT INTO public.stops VALUES (26, 'Деловой Центр', 55.95, 37.8);
INSERT INTO public.stops VALUES (8, 'Парк Сакуры', 55.7533, 37.8);
INSERT INTO public.stops VALUES (21, 'Западные Ворота', 55.9, 37.6);
INSERT INTO public.stops VALUES (29, 'Монтажный Завод', 55.8, 37.7);
INSERT INTO public.stops VALUES (66, 'Тест 5', 55.6, 37.8);


--
-- TOC entry 4919 (class 0 OID 0)
-- Dependencies: 219
-- Name: buses_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.buses_id_seq', 86, true);


--
-- TOC entry 4920 (class 0 OID 0)
-- Dependencies: 224
-- Name: color_palette_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.color_palette_id_seq', 28, true);


--
-- TOC entry 4921 (class 0 OID 0)
-- Dependencies: 227
-- Name: distances_new_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.distances_new_id_seq', 453, true);


--
-- TOC entry 4922 (class 0 OID 0)
-- Dependencies: 222
-- Name: render_settings_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.render_settings_id_seq', 1, true);


--
-- TOC entry 4923 (class 0 OID 0)
-- Dependencies: 217
-- Name: stops_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.stops_id_seq', 66, true);


--
-- TOC entry 4740 (class 2606 OID 16463)
-- Name: bus_stops bus_stops_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bus_stops
    ADD CONSTRAINT bus_stops_pkey PRIMARY KEY (bus_id, stop_position);


--
-- TOC entry 4736 (class 2606 OID 16413)
-- Name: buses buses_name_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.buses
    ADD CONSTRAINT buses_name_key UNIQUE (name);


--
-- TOC entry 4738 (class 2606 OID 16411)
-- Name: buses buses_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.buses
    ADD CONSTRAINT buses_pkey PRIMARY KEY (id);


--
-- TOC entry 4744 (class 2606 OID 16531)
-- Name: color_palette color_palette_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.color_palette
    ADD CONSTRAINT color_palette_pkey PRIMARY KEY (id);


--
-- TOC entry 4746 (class 2606 OID 16548)
-- Name: distances distances_new_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.distances
    ADD CONSTRAINT distances_new_pkey PRIMARY KEY (id);


--
-- TOC entry 4742 (class 2606 OID 16522)
-- Name: render_settings render_settings_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.render_settings
    ADD CONSTRAINT render_settings_pkey PRIMARY KEY (id);


--
-- TOC entry 4732 (class 2606 OID 16402)
-- Name: stops stops_name_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.stops
    ADD CONSTRAINT stops_name_key UNIQUE (name);


--
-- TOC entry 4734 (class 2606 OID 16400)
-- Name: stops stops_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.stops
    ADD CONSTRAINT stops_pkey PRIMARY KEY (id);


--
-- TOC entry 4747 (class 2606 OID 24749)
-- Name: bus_stops bus_stops_bus_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bus_stops
    ADD CONSTRAINT bus_stops_bus_id_fkey FOREIGN KEY (bus_id) REFERENCES public.buses(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 4748 (class 2606 OID 24754)
-- Name: bus_stops bus_stops_stop_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bus_stops
    ADD CONSTRAINT bus_stops_stop_id_fkey FOREIGN KEY (stop_id) REFERENCES public.stops(id) ON DELETE CASCADE;


--
-- TOC entry 4749 (class 2606 OID 16532)
-- Name: color_palette color_palette_render_setting_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.color_palette
    ADD CONSTRAINT color_palette_render_setting_id_fkey FOREIGN KEY (render_setting_id) REFERENCES public.render_settings(id) ON DELETE CASCADE;


--
-- TOC entry 4750 (class 2606 OID 24739)
-- Name: distances distances_new_from_stop_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.distances
    ADD CONSTRAINT distances_new_from_stop_id_fkey FOREIGN KEY (from_stop_id) REFERENCES public.stops(id) ON DELETE CASCADE;


-- Completed on 2024-12-17 10:49:40

--
-- PostgreSQL database dump complete
--

